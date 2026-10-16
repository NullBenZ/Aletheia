#include "PatternDetector.hpp"
#include <algorithm>
#include <cctype>
#include <regex>
#include <set>
using namespace std;
namespace fs = filesystem;
// Tests each extracted string against simple URL, address, email, and keyword patterns.
void PatternDetector::analyze(const fs::path&, FileReport& report) {
    const regex url("https?://[^\\s]+"), ip("(^|[^0-9])((25[0-5]|2[0-4][0-9]|1?[0-9]?[0-9])\\.){3}(25[0-5]|2[0-4][0-9]|1?[0-9]?[0-9])([^0-9]|$)"), email("[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}"); const regex keyword("cmd\\.exe|powershell|/bin/sh|/bin/bash|wget |curl |password|passwd|base64|eval\\(|chmod \\+x|\\.onion",regex::icase);
    map<string,set<string>> seen;map<string,size_t> counts; auto add=[&](const string& category,const string& value){++report.totalFindings;if(seen[category].insert(value).second&&counts[category]<20){report.findings.push_back({category,value});++counts[category];}};
    for(const string& text:report.allStrings){for(sregex_iterator it(text.begin(),text.end(),url),end;it!=end;++it)add("URL",it->str());for(sregex_iterator it(text.begin(),text.end(),ip),end;it!=end;++it)add("IPV4",(*it)[2].str());for(sregex_iterator it(text.begin(),text.end(),email),end;it!=end;++it)add("EMAIL",it->str());smatch match;if(regex_search(text,match,keyword))add("KEYWORD",match.str());}
    report.allStrings.clear();
}
// Returns the analyzer name.
string PatternDetector::name() const { return "Patterns"; }
