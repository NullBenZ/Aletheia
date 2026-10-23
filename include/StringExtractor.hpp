#ifndef ALETHEIA_STRING_EXTRACTOR_HPP
#define ALETHEIA_STRING_EXTRACTOR_HPP
#include "Analyzer.hpp"
class StringExtractor : public Analyzer {
public:
    explicit StringExtractor(std::size_t minimum = 4);
    void analyze(const std::filesystem::path &file, FileReport &report) override;

private:
    std::size_t minimum_;
};
#endif
