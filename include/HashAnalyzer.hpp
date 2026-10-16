#ifndef ALETHEIA_HASH_ANALYZER_HPP
#define ALETHEIA_HASH_ANALYZER_HPP
#include "Analyzer.hpp"
// Computes the file's SHA-256 digest in bounded memory.
class HashAnalyzer : public Analyzer {
public:
    void analyze(const std::filesystem::path& file, FileReport& report) override; // Streams file bytes through Sha256.
    std::string name() const override; // Names this analyzer.
};
#endif
