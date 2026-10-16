#ifndef ALETHEIA_ANALYZER_HPP
#define ALETHEIA_ANALYZER_HPP
#include "FileReport.hpp"
#include <filesystem>
#include <string>

// Defines the common interface used to run each file analyzer polymorphically.
class Analyzer {
public:
    virtual void analyze(const std::filesystem::path& file, FileReport& report) = 0; // Analyzes one file and stores observations.
    virtual std::string name() const = 0; // Returns the analyzer's display name.
    virtual ~Analyzer() = default;
};
#endif
