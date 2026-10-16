#include "ReportGenerator.hpp"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
using namespace std;
namespace fs = filesystem;
namespace {
// Adds thousands separators to a byte count.
string grouped(uintmax_t value){string text=to_string(value);for(int pos=static_cast<int>(text.size())-3;pos>0;pos-=3)text.insert(static_cast<size_t>(pos),",");return text;}
// Formats the current local date and time for folder summaries.
string nowText(){const time_t raw=time(nullptr);tm local{};localtime_r(&raw,&local);ostringstream out;out<<put_time(&local,"%Y-%m-%d %H:%M:%S");return out.str();}
}
// Prints one complete file analysis block.
void ReportGenerator::file(const FileReport& r,ostream& out)const{out<<"==================================================\n ALETHEIA  |  File analysis\n==================================================\n"<<"File:        "<<r.path.string()<<"\nSize:        "<<grouped(r.sizeBytes)<<" bytes\nExtension:   "<<(r.extension.empty()?"(none)":r.extension)<<"\nModified:    "<<r.lastModified<<"\nPermissions: "<<r.permissions<<"\n\nDetected type:  "<<r.detectedType<<'\n';if(r.extensionMismatch)out<<"[!] Extension mismatch: "<<r.mismatchNote<<'\n';out<<"\nSHA-256:\n  "<<r.sha256<<"\n\nStrings found: "<<r.stringCount<<" (min length 4)\n";for(const auto& s:r.sampleStrings)out<<"  e.g. \""<<s<<"\"\n";if(!r.stringsNote.empty())out<<"  "<<r.stringsNote<<'\n';if(r.findings.empty())out<<"\nPatterns: none\n";else{out<<"\nPatterns ("<<r.totalFindings<<" total):\n";for(const auto& f:r.findings)out<<"  ["<<f.category<<"]  "<<f.value<<'\n';}out<<"==================================================\n";}
// Prints all file blocks, duplicate groups, and the folder summary.
void ReportGenerator::folder(const vector<FileReport>& reports,const vector<DuplicateDetector::Group>& groups,const fs::path& target,size_t skipped,ostream& out)const{for(const auto& r:reports)file(r,out);out<<"\n--------------------------------------------------\n DUPLICATE GROUPS\n--------------------------------------------------\n";if(groups.empty())out<<"No duplicates found.\n";size_t index=1;for(const auto& g:groups){uintmax_t size=0;for(const auto& p:g)for(const auto& r:reports)if(r.path==p){size=r.sizeBytes;break;}out<<"Duplicate group #"<<index++<<"  ("<<g.size()<<" files, "<<grouped(size)<<" bytes each)\n  SHA-256: ";for(const auto& r:reports)if(r.path==g.front()){out<<r.sha256;break;}out<<'\n';for(const auto& p:g)out<<"  - "<<p.string()<<'\n';}size_t mismatch=0,withFindings=0;for(const auto& r:reports){if(r.extensionMismatch)++mismatch;if(r.totalFindings)++withFindings;}out<<"\n--------------------------------------------------\n SUMMARY\n--------------------------------------------------\nTarget:            "<<target.string()<<"\nGenerated:         "<<nowText()<<"\nFiles scanned:     "<<reports.size()<<"\nFiles skipped:     "<<skipped<<" (unreadable)\nExtension mismatches: "<<mismatch<<"\nFiles with pattern findings: "<<withFindings<<"\nDuplicate groups:  "<<groups.size()<<'\n';}
