#ifndef ALETHEIA_METADATA_ANALYZER_HPP
#define ALETHEIA_METADATA_ANALYZER_HPP
#include "Analyzer.hpp"
// Collects basic filesystem metadata without changing the input file.
class MetadataAnalyzer : public Analyzer {
public:
    void analyze(const std::filesystem::path& file, FileReport& report) override; // Fills metadata fields.
    std::string name() const override; // Names this analyzer.
};
#endif
