#include "HashAnalyzer.hpp"
#include "Sha256.hpp"
namespace fs = std::filesystem;
void HashAnalyzer::analyze(const fs::path &file, FileReport &report) {
    report.sha256 = sha256File(file);
}
