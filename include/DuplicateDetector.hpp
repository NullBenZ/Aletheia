#ifndef ALETHEIA_DUPLICATE_DETECTOR_HPP
#define ALETHEIA_DUPLICATE_DETECTOR_HPP
#include "FileReport.hpp"
#include <vector>
class DuplicateDetector {
public:
    using Group = std::vector<std::filesystem::path>;
    std::vector<Group> find(const std::vector<FileReport> &reports) const;
};
#endif
