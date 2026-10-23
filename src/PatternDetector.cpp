#include "PatternDetector.hpp"
#include <regex>
#include <set>

using std::map;
using std::regex;
using std::regex_search;
using std::set;
using std::size_t;
using std::smatch;
using std::sregex_iterator;
using std::string;
namespace fs = std::filesystem;
namespace {
const regex urlPattern("https?://[^\\s]+");
const regex ipPattern(
    "(^|[^0-9])((25[0-5]|2[0-4][0-9]|1?[0-9]?[0-9])\\.){3}(25[0-5]|2[0-4][0-9]|1?[0-9]?[0-9]"
    ")([^0-9]|$)");
const regex emailPattern("[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}");
const regex keywordPattern("cmd\\.exe|powershell|/bin/sh|/bin/bash|wget |curl "
                           "|password|passwd|base64|eval\\(|chmod \\+x|\\.onion",
                           regex::icase);
}

void PatternDetector::analyze(const fs::path &, FileReport &report) {
    map<string, set<string>> seen;
    map<string, size_t> counts;
    auto add = [&](const string &category, const string &value) {
        ++report.totalFindings;
        if (seen[category].insert(value).second && counts[category] < 20) {
            report.findings.push_back({category, value});
            ++counts[category];
        }
    };
    for (const string &text : report.allStrings) {
        for (sregex_iterator it(text.begin(), text.end(), urlPattern), end; it != end; ++it)
            add("URL", it->str());
        for (sregex_iterator it(text.begin(), text.end(), ipPattern), end; it != end; ++it)
            add("IPV4", (*it)[2].str());
        for (sregex_iterator it(text.begin(), text.end(), emailPattern), end; it != end; ++it)
            add("EMAIL", it->str());
        smatch match;
        if (regex_search(text, match, keywordPattern))
            add("KEYWORD", match.str());
    }
    report.allStrings.clear();
}
