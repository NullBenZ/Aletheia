#include "StringExtractor.hpp"
#include <algorithm>
#include <array>
#include <fstream>
#include <stdexcept>

using std::array;
using std::ifstream;
using std::ios;
using std::min;
using std::runtime_error;
using std::size_t;
using std::streamsize;
using std::string;
namespace fs = std::filesystem;
StringExtractor::StringExtractor(size_t minimum) : minimum_(minimum) {}
void StringExtractor::analyze(const fs::path &file, FileReport &report) {
    constexpr size_t byteLimit = 50U * 1024U * 1024U, stringLimit = 200000;
    array<char, 65536> buffer{};
    string current;
    size_t readBytes = 0;
    report.allStrings.clear();
    report.sampleStrings.clear();
    report.stringCount = 0;
    report.stringsNote.clear();

    const auto keepCurrent = [&] {
        if (current.size() < minimum_) {
            current.clear();
            return;
        }
        if (report.allStrings.size() < stringLimit)
            report.allStrings.push_back(current);
        ++report.stringCount;
        current.clear();
    };

    ifstream input(file, ios::binary);
    if (!input)
        throw runtime_error("cannot read file");
    while (readBytes < byteLimit && input) {
        const size_t ask = min(buffer.size(), byteLimit - readBytes);
        input.read(buffer.data(), static_cast<streamsize>(ask));
        const size_t got = static_cast<size_t>(input.gcount());
        readBytes += got;
        for (size_t i = 0; i < got; ++i) {
            const unsigned char ch = static_cast<unsigned char>(buffer[i]);
            if ((ch >= 32 && ch <= 126) || ch == 9)
                current.push_back(static_cast<char>(ch));
            else
                keepCurrent();
        }
    }
    keepCurrent();
    report.sampleStrings.assign(report.allStrings.begin(),
                                report.allStrings.begin() +
                                    min<size_t>(5, report.allStrings.size()));
    if (readBytes == byteLimit && report.sizeBytes > byteLimit)
        report.stringsNote = "strings truncated (50 MiB input cap)";
    if (report.stringCount > stringLimit) {
        report.allStrings.resize(stringLimit);
        report.stringsNote = "strings truncated (200,000 string cap)";
    }
}
