# Aletheia: viva notes

Aletheia is a small, read-only first-pass file triage program. It records observations such as file signatures, hashes, strings, and simple indicators. A match is a clue for review, not a conclusion about the file.

## Classes and connections

- **`FileReport` and `Finding`** (`include/FileReport.hpp`): Plain data structures. Every analyzer writes its observations into one `FileReport`; a `Finding` holds one category and value. `allStrings` temporarily shares extracted strings with `PatternDetector` and is cleared after pattern checks.
- **`Analyzer`** (`include/Analyzer.hpp`): Abstract base class defining `analyze` and `name`. It gives the manager one common interface for all analyzer types.
- **`MetadataAnalyzer`** (`include/MetadataAnalyzer.hpp`, `src/MetadataAnalyzer.cpp`): Fills path, size, lowercase extension, modification time, and permission text. It uses `std::filesystem` metadata.
- **`SignatureAnalyzer`** (`include/SignatureAnalyzer.hpp`, `src/SignatureAnalyzer.cpp`): Reads up to the first 16 bytes and compares them to a table of common file signatures. It sets a mismatch observation when an identified format conflicts with a present extension.
- **`Sha256`** (`include/Sha256.hpp`, `src/Sha256.cpp`): Implements SHA-256. `update` accepts successive chunks; `hexDigest` pads the final data and formats the digest. `HashAnalyzer` uses it.
- **`HashAnalyzer`** (`include/HashAnalyzer.hpp`, `src/HashAnalyzer.cpp`): Reads in 64 KiB chunks and stores the digest in `FileReport`.
- **`StringExtractor`** (`include/StringExtractor.hpp`, `src/StringExtractor.cpp`): Finds printable ASCII runs with a configurable minimum length. It stores all retained runs for the next analyzer and copies the first five into `sampleStrings`. It reads at most 50 MiB and stores at most 200,000 strings.
- **`PatternDetector`** (`include/PatternDetector.hpp`, `src/PatternDetector.cpp`): Applies URL, IPv4, email, and case-insensitive keyword checks to each extracted string. It stores up to 20 values per category and then clears the temporary strings.
- **`DuplicateDetector`** (`include/DuplicateDetector.hpp`, `src/DuplicateDetector.cpp`): Groups non-empty file reports by hash, sorts paths and groups, and returns groups with at least two members.
- **`AnalysisManager`** (`include/AnalysisManager.hpp`, `src/AnalysisManager.cpp`): Owns analyzers and applies them in order. Its folder method skips symbolic links and non-regular files, sorts paths, and counts per-file failures.
- **`ReportGenerator`** (`include/ReportGenerator.hpp`, `src/ReportGenerator.cpp`): Writes file details, duplicate groups, and a folder summary to an output stream. The CLI can give it either `cout` or a text file stream.

## OOP principles used

- **Inheritance:** `MetadataAnalyzer`, `SignatureAnalyzer`, `HashAnalyzer`, `StringExtractor`, and `PatternDetector` inherit from `Analyzer`; see their `include/*Analyzer.hpp` headers.
- **Virtual functions:** `Analyzer::analyze` and `Analyzer::name` are virtual in `include/Analyzer.hpp`.
- **`override`:** Each concrete analyzer marks its implementations with `override`, for example `include/MetadataAnalyzer.hpp`.
- **Polymorphism:** `AnalysisManager::analyzeFile` in `src/AnalysisManager.cpp` calls `analyzer->analyze(file, report)` through `unique_ptr<Analyzer>`. The actual analyzer implementation runs based on the pointed-to object.
- **`unique_ptr`:** `AnalysisManager` stores `vector<unique_ptr<Analyzer>>`. `make_unique` in its constructor creates ownership, and automatic destruction frees the analyzers without manual `delete`.
- **Encapsulation:** SHA-256 internal state is private in `include/Sha256.hpp`; callers use `update` and `hexDigest`.

## SHA-256 in plain language

SHA-256 first treats the file as bits and records the original bit length. It appends a `1` bit (the byte `0x80`), enough zero bits to leave eight bytes at the end of a 64-byte block, and then the original length as a 64-bit big-endian number. Each block is read as sixteen big-endian 32-bit words. The message schedule expands those into 64 words using rotations, shifts, and additions. The algorithm starts with eight standard state words and performs 64 rounds per block, mixing schedule words with choice, majority, and rotation functions plus fixed constants. The final eight state words form the digest. `src/Sha256.cpp` contains the implementation and comments on each stage.

## Likely viva questions

1. **Why is `Analyzer` abstract?** It defines common behavior but should not be instantiated; specific analyzer classes implement it.
2. **What is polymorphism here?** The manager calls the same virtual function through a base-class pointer, and each derived analyzer runs its own code.
3. **Why use `override`?** It lets the compiler verify that a method really overrides a base virtual function.
4. **What does `unique_ptr` provide?** Exclusive automatic ownership, so analyzer objects are released when the manager is destroyed.
5. **Why read files in chunks?** Memory use stays bounded even for large evidence files.
6. **Why use magic bytes?** File contents can identify a format even when a filename extension is wrong.
7. **Why compute SHA-256?** It gives a repeatable fingerprint useful for checking integrity and finding identical files.
8. **What does SHA padding do?** It marks the message end, fills a 512-bit block correctly, and includes the original length.
9. **What does `StringExtractor` pass to `PatternDetector`?** It shares capped strings through `FileReport::allStrings`, avoiding another file read for that task.
10. **Are pattern findings verdicts?** No. They are simple observations that need human interpretation.

## Chosen scope details

Folder traversal reports filesystem iteration errors as skipped items and continues where the iterator allows. Keyword detection records the first matching keyword from each extracted string. The report says the string minimum is four for the standard analyzer; the standalone `strings --min` command accepts a custom value.
