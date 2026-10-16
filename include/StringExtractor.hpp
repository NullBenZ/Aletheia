#ifndef ALETHEIA_STRING_EXTRACTOR_HPP
#define ALETHEIA_STRING_EXTRACTOR_HPP
#include "Analyzer.hpp"
// Extracts printable ASCII runs from the first 50 MiB of a file.
class StringExtractor : public Analyzer {
public:
    explicit StringExtractor(std::size_t minimum = 4); // Sets the minimum retained string length.
    void analyze(const std::filesystem::path& file, FileReport& report) override; // Stores samples and strings for PatternDetector.
    std::string name() const override; // Names this analyzer.
private:
    std::size_t minimum_;
};
#endif
