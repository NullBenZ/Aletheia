#include "DuplicateDetector.hpp"
#include <algorithm>
#include <unordered_map>
using namespace std;
namespace fs = filesystem;
// Groups reports by digest, excluding empty files and empty hashes.
vector<DuplicateDetector::Group> DuplicateDetector::find(const vector<FileReport>& reports) const { unordered_map<string,Group> byHash;for(const auto& report:reports)if(report.sizeBytes>0&&!report.sha256.empty())byHash[report.sha256].push_back(report.path);vector<Group> groups;for(auto& item:byHash)if(item.second.size()>=2){sort(item.second.begin(),item.second.end());groups.push_back(move(item.second));}sort(groups.begin(),groups.end(),[](const Group& a,const Group& b){return a.front()<b.front();});return groups; }
