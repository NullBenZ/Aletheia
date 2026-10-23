#include "AnalysisManager.hpp"
#include "DuplicateDetector.hpp"
#include "ReportGenerator.hpp"
#include "Sha256.hpp"
#include "StringExtractor.hpp"

#include <charconv>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <system_error>

namespace {

namespace fs = std::filesystem;

void printUsage(std::ostream &out) {
    out << "Usage:\n"
           "  aletheia analyze <file>\n"
           "  aletheia scan <folder>\n"
           "  aletheia hash <file>\n"
           "  aletheia strings <file> [--min N]\n"
           "  aletheia duplicates <folder>\n"
           "  aletheia report <folder> --output <file>\n"
           "  aletheia help\n"
           "  aletheia version\n";
}

void requirePath(const fs::path &path, bool directory) {
    std::error_code error;
    if (!fs::exists(path, error))
        throw std::runtime_error("path does not exist: " + path.string());

    const bool correctType =
        directory ? fs::is_directory(path, error) : fs::is_regular_file(path, error);
    if (!correctType)
        throw std::runtime_error("path is not a " +
                                 std::string(directory ? "folder: " : "regular file: ") +
                                 path.string());
}

int analyzeFile(const fs::path &path) {
    requirePath(path, false);

    AnalysisManager manager;
    ReportGenerator report;
    report.file(manager.analyzeFile(path), std::cout);
    return 0;
}

int hashFile(const fs::path &path) {
    requirePath(path, false);
    std::cout << sha256File(path) << "  " << path.string() << '\n';
    return 0;
}

std::size_t parseMinimumLength(const char *text) {
    const std::string value(text);
    std::size_t minimum = 0;
    const auto result = std::from_chars(value.data(), value.data() + value.size(), minimum);
    if (value.empty() || result.ec != std::errc{} || result.ptr != value.data() + value.size() ||
        minimum == 0)
        throw std::runtime_error("bad --min value");
    return minimum;
}

int printStrings(const fs::path &path, int argc, char *argv[]) {
    requirePath(path, false);

    std::size_t minimum = 4;
    if (argc == 5 && std::string(argv[3]) == "--min")
        minimum = parseMinimumLength(argv[4]);
    else if (argc != 3)
        throw std::runtime_error("bad strings arguments");

    FileReport report;
    report.sizeBytes = fs::file_size(path);
    StringExtractor extractor(minimum);
    extractor.analyze(path, report);

    for (const auto &text : report.allStrings)
        std::cout << text << '\n';
    return 0;
}

void ensureOutputOutsideFolder(const fs::path &folder, const fs::path &output) {
    const fs::path parent = output.parent_path().empty() ? fs::path(".") : output.parent_path();
    const fs::path canonicalFolder = fs::weakly_canonical(folder);
    const fs::path canonicalOutput = fs::weakly_canonical(parent) / output.filename();
    const fs::path relativeOutput = canonicalOutput.lexically_relative(canonicalFolder);

    if (relativeOutput.empty() || *relativeOutput.begin() != "..")
        throw std::runtime_error("report output must be outside the analyzed folder");
    fs::create_directories(parent);
}

int scanFolder(const fs::path &folder, const fs::path &output, bool writeReport) {
    requirePath(folder, true);
    if (writeReport)
        ensureOutputOutsideFolder(folder, output);

    AnalysisManager manager;
    std::size_t skipped = 0;
    const auto reports = manager.analyzeFolder(folder, skipped);

    DuplicateDetector detector;
    const auto duplicates = detector.find(reports);
    ReportGenerator report;

    if (writeReport) {
        std::ofstream out(output);
        if (!out)
            throw std::runtime_error("cannot write report: " + output.string());
        report.folder(reports, duplicates, folder, skipped, out);
    } else {
        report.folder(reports, duplicates, folder, skipped, std::cout);
    }
    return 0;
}

int listDuplicates(const fs::path &folder) {
    requirePath(folder, true);

    AnalysisManager manager;
    std::size_t skipped = 0;
    const auto reports = manager.analyzeFolder(folder, skipped);
    DuplicateDetector detector;
    const auto groups = detector.find(reports);

    if (groups.empty())
        std::cout << "No duplicates found.\n";

    std::size_t index = 1;
    for (const auto &group : groups) {
        std::cout << "Duplicate group #" << index++ << " (" << group.size() << " files)\n";
        for (const auto &path : group)
            std::cout << "  - " << path.string() << '\n';
    }
    return 0;
}

int runCommand(int argc, char *argv[]) {
    if (argc == 2 && std::string(argv[1]) == "help") {
        printUsage(std::cout);
        return 0;
    }
    if (argc == 2 && std::string(argv[1]) == "version") {
        std::cout << "Aletheia 1.0.0\n";
        return 0;
    }
    if (argc == 3 && std::string(argv[1]) == "analyze")
        return analyzeFile(argv[2]);
    if (argc == 3 && std::string(argv[1]) == "hash")
        return hashFile(argv[2]);
    if (argc == 3 && std::string(argv[1]) == "strings")
        return printStrings(argv[2], argc, argv);
    if (argc == 5 && std::string(argv[1]) == "strings" && std::string(argv[3]) == "--min")
        return printStrings(argv[2], argc, argv);
    if (argc == 3 && std::string(argv[1]) == "scan")
        return scanFolder(argv[2], {}, false);
    if (argc == 3 && std::string(argv[1]) == "duplicates")
        return listDuplicates(argv[2]);
    if (argc == 5 && std::string(argv[1]) == "report" && std::string(argv[3]) == "--output")
        return scanFolder(argv[2], argv[4], true);

    std::cerr << "Unknown command or missing/invalid arguments.\n";
    printUsage(std::cerr);
    return 1;
}

}

int main(int argc, char *argv[]) {
    try {
        return runCommand(argc, argv);
    } catch (const std::exception &error) {
        std::cerr << "Error: " << error.what() << '\n';
        printUsage(std::cerr);
        return 1;
    }
}
