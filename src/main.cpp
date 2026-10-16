#include "AnalysisManager.hpp"
#include "DuplicateDetector.hpp"
#include "ReportGenerator.hpp"
#include "Sha256.hpp"
#include "StringExtractor.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
using namespace std;
namespace fs = filesystem;

// Prints the supported commands and their argument forms.
static void printUsage(ostream& out) { out << "Usage:\n  aletheia analyze <file>\n  aletheia scan <folder>\n  aletheia hash <file>\n  aletheia strings <file> [--min N]\n  aletheia duplicates <folder>\n  aletheia report <folder> --output <file>\n  aletheia help\n  aletheia version\n"; }
// Rejects missing paths and paths of the wrong filesystem kind.
static void requirePath(const fs::path& path, bool directory) { error_code ec; if(!fs::exists(path,ec))throw runtime_error("path does not exist: "+path.string()); if(directory?!fs::is_directory(path,ec):!fs::is_regular_file(path,ec))throw runtime_error(string("path is not a ")+(directory?"folder: ":"regular file: ")+path.string()); }
// Analyzes one input and writes its formatted report to standard output.
static int analyzeCommand(const fs::path& path) { requirePath(path,false);AnalysisManager manager;ReportGenerator printer;printer.file(manager.analyzeFile(path),cout);return 0; }
// Streams a single file through the SHA-256 implementation and prints its path.
static int hashCommand(const fs::path& path) { requirePath(path,false);ifstream input(path,ios::binary);if(!input)throw runtime_error("cannot read file: "+path.string());Sha256 hash;char buffer[65536];while(input){input.read(buffer,sizeof(buffer));auto n=input.gcount();if(n>0)hash.update(reinterpret_cast<const uint8_t*>(buffer),static_cast<size_t>(n));}if(input.bad())throw runtime_error("error reading file: "+path.string());cout<<hash.hexDigest()<<"  "<<path.string()<<'\n';return 0; }
// Prints extracted ASCII strings with a validated minimum length.
static int stringsCommand(const fs::path& path,int argc,char* argv[]) { requirePath(path,false);size_t minimum=4;if(argc==5&&string(argv[3])=="--min"){try{const string value=argv[4];if(value.empty()||value.find_first_not_of("0123456789")!=string::npos)throw invalid_argument("minimum");size_t used=0;const auto parsed=stoull(value,&used);if(used!=value.size()||parsed==0||parsed>numeric_limits<size_t>::max())throw invalid_argument("minimum");minimum=static_cast<size_t>(parsed);}catch(...){throw runtime_error("bad --min value");}}else if(argc!=3)throw runtime_error("bad strings arguments");FileReport report;report.sizeBytes=fs::file_size(path);StringExtractor extractor(minimum);extractor.analyze(path,report);for(const auto& text:report.allStrings)cout<<text<<'\n';return 0; }
// Scans a folder and optionally writes the same text report to a file.
static int folderCommand(const fs::path& folder,const fs::path& output,bool writeReport) { requirePath(folder,true);if(writeReport){const fs::path parent=output.parent_path().empty()?fs::path("."):output.parent_path();const fs::path base=fs::weakly_canonical(folder),dest=fs::weakly_canonical(parent)/output.filename();const auto rel=dest.lexically_relative(base);if(rel.empty()||*rel.begin()!="..")throw runtime_error("report output must be outside the analyzed folder");fs::create_directories(parent);}AnalysisManager manager;size_t skipped=0;auto reports=manager.analyzeFolder(folder,skipped);DuplicateDetector detector;auto groups=detector.find(reports);ReportGenerator printer;if(writeReport){ofstream out(output);if(!out)throw runtime_error("cannot write report: "+output.string());printer.folder(reports,groups,folder,skipped,out);}else printer.folder(reports,groups,folder,skipped,cout);return 0; }
// Prints only duplicate file groups for the selected folder.
static int duplicatesCommand(const fs::path& folder) { requirePath(folder,true);AnalysisManager manager;size_t skipped=0;auto reports=manager.analyzeFolder(folder,skipped);DuplicateDetector detector;auto groups=detector.find(reports);if(groups.empty())cout<<"No duplicates found.\n";size_t n=1;for(const auto& group:groups){cout<<"Duplicate group #"<<n++<<" ("<<group.size()<<" files)\n";for(const auto& path:group)cout<<"  - "<<path.string()<<'\n';}return 0; }
// Validates command arguments and dispatches to the small command handlers.
int main(int argc,char* argv[]) {try{if(argc==2&&string(argv[1])=="help"){printUsage(cout);return 0;}if(argc==2&&string(argv[1])=="version"){cout<<"Aletheia 1.0.0\n";return 0;}if(argc==3&&string(argv[1])=="analyze")return analyzeCommand(argv[2]);if(argc==3&&string(argv[1])=="hash")return hashCommand(argv[2]);if((argc==3||(argc==5&&string(argv[3])=="--min"))&&string(argv[1])=="strings")return stringsCommand(argv[2],argc,argv);if(argc==3&&string(argv[1])=="scan")return folderCommand(argv[2],{},false);if(argc==3&&string(argv[1])=="duplicates")return duplicatesCommand(argv[2]);if(argc==5&&string(argv[1])=="report"&&string(argv[3])=="--output")return folderCommand(argv[2],argv[4],true);cerr<<"Unknown command or missing/invalid arguments.\n";printUsage(cerr);return 1;}catch(const exception& error){cerr<<"Error: "<<error.what()<<'\n';printUsage(cerr);return 1;}}
