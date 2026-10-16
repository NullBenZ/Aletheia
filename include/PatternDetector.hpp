#ifndef ALETHEIA_PATTERN_DETECTOR_HPP
#define ALETHEIA_PATTERN_DETECTOR_HPP
#include "Analyzer.hpp"
// Finds simple indicator patterns within extracted strings.
class PatternDetector : public Analyzer {
public:
    void analyze(const std::filesystem::path& file, FileReport& report) override; // Records capped, unique findings.
    std::string name() const override; // Names this analyzer.
};
#endif
