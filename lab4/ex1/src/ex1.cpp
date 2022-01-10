#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;
using namespace chrono;
using namespace filesystem;

void find_next(vector<int>& next, string t) {
    next.resize(t.length());
    for (int q = 1, k = 0; q < t.length(); ++q) {
        while (k && t[k] != t[q]) {
            k = next[k - 1];
        }
        if (t[k] == t[q]) {
            ++k;
        }
        next[q] = k;
    }
}

void KMP(string t, string s, vector<int>& position, vector<int> next) {
    auto n = s.length();
    auto m = t.length();
    find_next(next, t);
    for (int i = 0, q = 0; i < n; ++i) {
        while (q && t[q] != s[i]) {
            q = next[q - 1];
        }
        if (t[q] == s[i]) {
            ++q;
        }
        if (q == m) {
            position.push_back(i - m + 1);
            q = next[q - 1];
        }
    }
}

int main() {
    path p          = __FILE__;
    auto inputfile  = p.parent_path().parent_path();
    auto timefile   = p.parent_path().parent_path();
    auto resultfile = p.parent_path().parent_path();
    timefile += "\\output\\time.txt";
    resultfile += "\\output\\result.txt";
    vector<string> input;
    string         tmp;
    inputfile += "\\input\\4_1_input.txt";
    ifstream file;
    ofstream outputtime;
    ofstream output;
    file.open(inputfile, ios::in);
    outputtime.open(timefile, ios::out);
    output.open(resultfile, ios::out);

    while (getline(file, tmp)) {
        input.push_back(tmp);
    }
    vector<int> position;
    vector<int> next;
    for (int i = 0; i < input.size(); i += 2) {
        next.clear();
        steady_clock::time_point t1 = steady_clock::now();
        find_next(next, input[i]);
        steady_clock::time_point t2   = steady_clock::now();
        auto                     time = t2 - t1;
        outputtime << "next: " << duration<double, micro>(time).count() << "us";
        position.clear();
        t1 = steady_clock::now();
        KMP(input[i], input[i + 1], position, next);
        t2   = steady_clock::now();
        time = t2 - t1;
        outputtime << " kmp: " << duration<double, micro>(time).count() << "us"
                   << endl;
        output << position.size() << endl;
        for (int j = 0; j < next.size(); j++) {
            output << next[j] << " ";
        }
        output << endl;
        for (int j = 0; j < position.size(); j++) {
            output << position[j] + 1 << " ";
        }
        output << endl;
    }
}