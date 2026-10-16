#ifndef ALETHEIA_SIGNATURE_ANALYZER_HPP
#define ALETHEIA_SIGNATURE_ANALYZER_HPP
#include "Analyzer.hpp"
// Identifies common file formats from their leading bytes.
class SignatureAnalyzer : public Analyzer {
public:
    void analyze(const std::filesystem::path& file, FileReport& report) override; // Detects type and extension mismatch.
    std::string name() const override; // Names this analyzer.
};
#endif
