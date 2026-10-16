#ifndef ALETHEIA_DUPLICATE_DETECTOR_HPP
#define ALETHEIA_DUPLICATE_DETECTOR_HPP
#include "FileReport.hpp"
#include <vector>
// Groups non-empty files whose SHA-256 digests are equal.
class DuplicateDetector {
public:
    using Group = std::vector<std::filesystem::path>;
    std::vector<Group> find(const std::vector<FileReport>& reports) const; // Returns deterministically ordered duplicate groups.
};
#endif
