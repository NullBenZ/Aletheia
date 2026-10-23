cmake_minimum_required(VERSION 3.10)
project(aletheia LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_executable(aletheia
    src/AnalysisManager.cpp
    src/DuplicateDetector.cpp
    src/HashAnalyzer.cpp
    src/MetadataAnalyzer.cpp
    src/PatternDetector.cpp
    src/ReportGenerator.cpp
    src/Sha256.cpp
    src/SignatureAnalyzer.cpp
    src/StringExtractor.cpp
    src/main.cpp
)
target_include_directories(aletheia PRIVATE include)
target_compile_options(aletheia PRIVATE -Wall -Wextra)
