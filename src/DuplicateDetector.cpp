#include "DuplicateDetector.hpp"
#include <algorithm>
#include <unordered_map>

using std::move;
using std::sort;
using std::string;
using std::unordered_map;
using std::vector;
vector<DuplicateDetector::Group> DuplicateDetector::find(const vector<FileReport> &reports) const {
    unordered_map<string, Group> byHash;
    for (const auto &report : reports)
        if (report.sizeBytes > 0 && !report.sha256.empty())
            byHash[report.sha256].push_back(report.path);
    vector<Group> groups;
    for (auto &item : byHash)
        if (item.second.size() >= 2) {
            sort(item.second.begin(), item.second.end());
            groups.push_back(move(item.second));
        }
    sort(groups.begin(), groups.end(),
         [](const Group &a, const Group &b) { return a.front() < b.front(); });
    return groups;
}
