#include "SignatureAnalyzer.hpp"
#include <algorithm>
#include <array>
#include <fstream>

using std::array;
using std::equal;
using std::find;
using std::ifstream;
using std::ios;
using std::size_t;
using std::string;
using std::vector;
namespace fs = std::filesystem;

namespace {
struct Signature {
    vector<unsigned char> bytes;
    string type;
    vector<string> extensions;
};

const vector<Signature> signatures = {
    {{0x53, 0x51, 0x4c, 0x69, 0x74, 0x65, 0x20, 0x66, 0x6f, 0x72, 0x6d, 0x61, 0x74, 0x20, 0x33,
      0x00},
     "SQLite database",
     {".db", ".sqlite"}},
    {{0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a}, "PNG image", {".png"}},
    {{0x37, 0x7a, 0xbc, 0xaf, 0x27, 0x1c}, "7-Zip archive", {".7z"}},
    {{0x52, 0x61, 0x72, 0x21, 0x1a, 0x07}, "RAR archive", {".rar"}},
    {{0x7f, 0x45, 0x4c, 0x46}, "Linux executable", {}},
    {{0x50, 0x4b, 0x03, 0x04}, "ZIP archive", {".zip", ".docx", ".xlsx", ".pptx", ".jar", ".apk"}},
    {{0xff, 0xd8, 0xff}, "JPEG image", {".jpg", ".jpeg"}},
    {{0x25, 0x50, 0x44, 0x46}, "PDF document", {".pdf"}},
    {{0x47, 0x49, 0x46, 0x38}, "GIF image", {".gif"}},
    {{0x4d, 0x5a}, "Windows executable", {".exe", ".dll"}},
    {{0x1f, 0x8b}, "gzip archive", {".gz", ".tgz"}}};

}

void SignatureAnalyzer::analyze(const fs::path &file, FileReport &report) {
    array<unsigned char, 16> bytes{};
    ifstream input(file, ios::binary);
    input.read(reinterpret_cast<char *>(bytes.data()), bytes.size());
    const auto count = static_cast<size_t>(input.gcount());
    report.detectedType = "Unknown";
    for (const auto &entry : signatures)
        if (count >= entry.bytes.size() &&
            equal(entry.bytes.begin(), entry.bytes.end(), bytes.begin())) {
            report.detectedType = entry.type;
            if (!report.extension.empty() && find(entry.extensions.begin(), entry.extensions.end(),
                                                  report.extension) == entry.extensions.end()) {
                report.extensionMismatch = true;
                report.mismatchNote =
                    "file is named " + report.extension + " but looks like " + entry.type;
            }
            break;
        }
}
