#ifndef ALETHEIA_ANALYSIS_MANAGER_HPP
#define ALETHEIA_ANALYSIS_MANAGER_HPP
#include "Analyzer.hpp"
#include <memory>
#include <vector>
// Owns analyzers and applies them in the required fixed order.
class AnalysisManager {
public:
    AnalysisManager(); // Registers all analyzers in processing order.
    FileReport analyzeFile(const std::filesystem::path& file); // Runs each analyzer polymorphically.
    std::vector<FileReport> analyzeFolder(const std::filesystem::path& folder, std::size_t& skipped); // Scans regular files and counts failures.
private:
    std::vector<std::unique_ptr<Analyzer>> analyzers_;
};
#endif
