# Aletheia

Aletheia is a command-line, read-only first-pass file analyzer written in C++17. It reports file metadata, likely format from signature bytes, a SHA-256 digest, printable ASCII strings, and simple indicators such as URLs, email addresses, IPv4 addresses, and selected keywords. Folder scans can also group identical files by hash.

These results are observations for review, not a malware verdict. Aletheia does not modify analyzed files.

## Build

Requirements: a C++17 compiler and CMake 3.10 or newer.

From the project directory:

```sh
cmake -S . -B build
cmake --build build
```

The executable is `build/aletheia` (on Windows, `build/Debug/aletheia.exe` may be used depending on the generator).

## Use

Run `./build/aletheia help` to print the command list. Examples:

```sh
./build/aletheia analyze samples/tiny.png
./build/aletheia scan samples
./build/aletheia hash samples/tiny.png
./build/aletheia strings samples/readme.txt --min 6
./build/aletheia duplicates samples
./build/aletheia report samples --output /tmp/aletheia-report.txt
```

Commands:

- `analyze <file>` prints the full analysis for one file.
- `scan <folder>` analyzes regular files recursively and prints a summary.
- `hash <file>` prints the file's SHA-256 digest.
- `strings <file> [--min N]` prints printable ASCII runs; the default minimum is 4 characters.
- `duplicates <folder>` lists groups with matching SHA-256 hashes.
- `report <folder> --output <file>` writes the folder scan report to a text file. Keep the output file outside the scanned folder.
- `version` prints the version; `help` prints usage.

Paths may be relative to the current directory or absolute. Folder scans skip symbolic links and non-regular files, and report files that could not be analyzed. String extraction is bounded to the first 50 MiB of a file and at most 200,000 strings; pattern results are capped per category. The project includes a small sample folder for trying the commands.

## How it works

`main.cpp` validates command arguments and dispatches to the requested operation. For file analysis, `AnalysisManager` runs a sequence of analyzer classes against the same `FileReport`: metadata, signature, SHA-256, strings, and pattern checks. `DuplicateDetector` groups folder results by digest, and `ReportGenerator` formats them for the terminal or an output file. The analyzers share a common `Analyzer` interface, so the manager can run each analyzer polymorphically.

The SHA-256 implementation is included in the project; it reads files in chunks so hashing does not require loading an entire file into memory. Signature and pattern matches are simple indicators and may be incomplete or incorrect.

## Tests

Run the included test script from the project directory:

```sh
bash tests/run_tests.sh
```

