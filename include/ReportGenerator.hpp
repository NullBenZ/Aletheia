#ifndef ALETHEIA_REPORT_GENERATOR_HPP
#define ALETHEIA_REPORT_GENERATOR_HPP
#include "DuplicateDetector.hpp"
#include "FileReport.hpp"
#include <iosfwd>
#include <vector>
class ReportGenerator {
public:
    void file(const FileReport &report, std::ostream &out) const;
    void folder(const std::vector<FileReport> &reports,
                const std::vector<DuplicateDetector::Group> &groups,
                const std::filesystem::path &target, std::size_t skipped, std::ostream &out) const;
};
#endif
