#ifndef ALETHEIA_ANALYZER_HPP
#define ALETHEIA_ANALYZER_HPP
#include "FileReport.hpp"
#include <filesystem>
#include <string>

class Analyzer {
public:
    virtual void analyze(const std::filesystem::path &file, FileReport &report) = 0;
    virtual ~Analyzer() = default;
};
#endif
