#include "AnalysisManager.hpp"
#include "HashAnalyzer.hpp"
#include "MetadataAnalyzer.hpp"
#include "PatternDetector.hpp"
#include "SignatureAnalyzer.hpp"
#include "StringExtractor.hpp"
#include <algorithm>
#include <iostream>
#include <system_error>

using std::cerr;
using std::error_code;
using std::exception;
using std::make_unique;
using std::size_t;
using std::sort;
using std::vector;
namespace fs = std::filesystem;
AnalysisManager::AnalysisManager() {
    analyzers_.push_back(make_unique<MetadataAnalyzer>());
    analyzers_.push_back(make_unique<SignatureAnalyzer>());
    analyzers_.push_back(make_unique<HashAnalyzer>());
    analyzers_.push_back(make_unique<StringExtractor>());
    analyzers_.push_back(make_unique<PatternDetector>());
}
FileReport AnalysisManager::analyzeFile(const fs::path &file) {
    FileReport report;
    for (const auto &analyzer : analyzers_)
        analyzer->analyze(file, report);
    return report;
}
vector<FileReport> AnalysisManager::analyzeFolder(const fs::path &folder, size_t &skipped) {
    vector<fs::path> paths;
    fs::recursive_directory_iterator it(folder, fs::directory_options::skip_permission_denied), end;
    while (it != end) {
        try {
            if (it->is_symlink()) {
                if (it->is_directory())
                    it.disable_recursion_pending();
            } else if (it->is_regular_file())
                paths.push_back(it->path());
        } catch (const fs::filesystem_error &e) {
            cerr << "Warning: " << e.what() << '\n';
            ++skipped;
        }
        error_code ec;
        it.increment(ec);
        if (ec) {
            cerr << "Warning: " << ec.message() << '\n';
            ++skipped;
        }
    }
    sort(paths.begin(), paths.end());
    vector<FileReport> reports;
    for (const auto &path : paths) {
        try {
            reports.push_back(analyzeFile(path));
        } catch (const exception &e) {
            cerr << "Warning: skipping " << path << ": " << e.what() << '\n';
            ++skipped;
        }
    }
    return reports;
}
