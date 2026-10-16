#include "MetadataAnalyzer.hpp"
#include <algorithm>
#include <cctype>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
using namespace std;
namespace fs = filesystem;
// Converts a file timestamp and permission bits into readable metadata.
void MetadataAnalyzer::analyze(const fs::path& file, FileReport& report) {
    report.path = file; report.sizeBytes = fs::file_size(file); report.extension = file.extension().string();
    transform(report.extension.begin(), report.extension.end(), report.extension.begin(), [](unsigned char c) { return static_cast<char>(tolower(c)); });
    const auto fileTime = fs::last_write_time(file);
    // C++17 lacks a direct file-clock conversion, so preserve the current offset between clocks.
    const auto systemTime = chrono::time_point_cast<chrono::system_clock::duration>(fileTime - fs::file_time_type::clock::now() + chrono::system_clock::now());
    const time_t raw = chrono::system_clock::to_time_t(systemTime); tm local{}; localtime_r(&raw, &local);
    ostringstream date; date << put_time(&local, "%Y-%m-%d %H:%M:%S"); report.lastModified = date.str();
    const auto perms = fs::status(file).permissions(); const fs::perms bits[] = {fs::perms::owner_read, fs::perms::owner_write, fs::perms::owner_exec, fs::perms::group_read, fs::perms::group_write, fs::perms::group_exec, fs::perms::others_read, fs::perms::others_write, fs::perms::others_exec};
    const char chars[] = "rwxrwxrwx"; report.permissions.clear(); for (int i=0;i<9;++i) report.permissions += (perms & bits[i]) != fs::perms::none ? chars[i] : '-';
}
// Returns the analyzer name.
string MetadataAnalyzer::name() const { return "Metadata"; }
