#ifndef ALETHEIA_PATTERN_DETECTOR_HPP
#define ALETHEIA_PATTERN_DETECTOR_HPP
#include "Analyzer.hpp"
class PatternDetector : public Analyzer {
public:
    void analyze(const std::filesystem::path &file, FileReport &report) override;
};
#endif
