#include <iostream>
#include <fstream>
#include <deque>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <vector>


using namespace std;

int main() {
    for (int i = 1; i < 6; ++i) {
        int l = 0;
        int r = i;
        ifstream in("trace.log");
        if (!in) {
            cerr << "Cannot open trace.log" << endl;
            return 1;
        }
        deque<string> q;
        string line;
        for (int j = l; j < r; ++j) {
            getline(in, line);
            q.push_back(line);
        }
        unordered_map<string, int> um;
        string key = "";
        for (const auto& l : q) {
            key += l;
            key += ";";
        }
        key.pop_back();
        um[key]++;
        while(getline(in, line)) {
            q.pop_front();
            q.push_back(line);
            key = "";
            for (const auto& l : q) {
                key += l;
                key += ";";
            }
            key.pop_back();
            um[key]++;
        }
        string name = "trace";
        name += to_string(i);
        name += ".log";
        ofstream out(name);
        if (!out) {
            cerr << "Cannot open " << name << endl;
            return 2;
        }
        vector<pair<string, int>> res;
        for (auto it = um.begin(); it != um.end(); ++it) {
            res.push_back({it->first, it->second});
        }
        sort(res.begin(), res.end(), [](auto& a, auto& b) { return a.second > b.second; });
        int kMax = min((int)res.size(), 20);
        for (int k = 0; k < kMax; ++k) {
            out << res[k].second << ":" << res[k].first << endl;
        }
    }

    return 0;
}