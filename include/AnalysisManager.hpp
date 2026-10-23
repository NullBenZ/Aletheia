#ifndef ALETHEIA_ANALYSIS_MANAGER_HPP
#define ALETHEIA_ANALYSIS_MANAGER_HPP
#include "Analyzer.hpp"
#include <memory>
#include <vector>
class AnalysisManager {
public:
    AnalysisManager();
    FileReport analyzeFile(const std::filesystem::path &file);
    std::vector<FileReport> analyzeFolder(const std::filesystem::path &folder,
                                          std::size_t &skipped);

private:
    std::vector<std::unique_ptr<Analyzer>> analyzers_;
};
#endif
