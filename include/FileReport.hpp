#ifndef ALETHEIA_FILE_REPORT_HPP
#define ALETHEIA_FILE_REPORT_HPP

#include <filesystem>
#include <string>
#include <vector>

struct Finding {
    std::string category;
    std::string value;
};

struct FileReport {
    std::filesystem::path path;
    std::uintmax_t sizeBytes = 0;
    std::string extension;
    std::string lastModified;
    std::string permissions;
    std::string detectedType;
    bool extensionMismatch = false;
    std::string mismatchNote;
    std::string sha256;
    std::size_t stringCount = 0;
    std::vector<std::string> sampleStrings;
    std::vector<std::string> allStrings;
    std::string stringsNote;
    std::vector<Finding> findings;
    std::size_t totalFindings = 0;
};

#endif
