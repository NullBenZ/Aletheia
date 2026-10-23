#ifndef ALETHEIA_METADATA_ANALYZER_HPP
#define ALETHEIA_METADATA_ANALYZER_HPP
#include "Analyzer.hpp"
class MetadataAnalyzer : public Analyzer {
public:
    void analyze(const std::filesystem::path &file, FileReport &report) override;
};
#endif
