#ifndef ALETHEIA_HASH_ANALYZER_HPP
#define ALETHEIA_HASH_ANALYZER_HPP
#include "Analyzer.hpp"
class HashAnalyzer : public Analyzer {
public:
    void analyze(const std::filesystem::path &file, FileReport &report) override;
};
#endif
