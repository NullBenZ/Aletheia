#include "HashAnalyzer.hpp"
#include "Sha256.hpp"
#include <array>
#include <fstream>
#include <stdexcept>
using namespace std;
namespace fs = filesystem;
// Reads the file in 64 KiB chunks and computes SHA-256.
void HashAnalyzer::analyze(const fs::path& file, FileReport& report) { ifstream input(file, ios::binary); if(!input) throw runtime_error("cannot read file"); Sha256 hash; array<uint8_t,65536> buffer{}; while(input){input.read(reinterpret_cast<char*>(buffer.data()),buffer.size());const auto got=input.gcount();if(got>0)hash.update(buffer.data(),static_cast<size_t>(got));}if(input.bad())throw runtime_error("error reading file");report.sha256=hash.hexDigest(); }
// Returns the analyzer name.
string HashAnalyzer::name() const { return "SHA-256"; }
