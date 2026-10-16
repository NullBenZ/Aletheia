#ifndef ALETHEIA_REPORT_GENERATOR_HPP
#define ALETHEIA_REPORT_GENERATOR_HPP
#include "DuplicateDetector.hpp"
#include "FileReport.hpp"
#include <iosfwd>
#include <vector>
// Writes readable analysis blocks, duplicate groups, and folder summaries.
class ReportGenerator {
public:
    void file(const FileReport& report, std::ostream& out) const; // Formats one file.
    void folder(const std::vector<FileReport>& reports, const std::vector<DuplicateDetector::Group>& groups, const std::filesystem::path& target, std::size_t skipped, std::ostream& out) const; // Formats files and summary.
};
#endif
