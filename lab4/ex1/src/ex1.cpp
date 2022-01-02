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
    next.resize(t.size());
    next[0] = -1;
    for (int i = 1; i < t.size(); i++) {
        int j = next[i - 1];
        while (t[i + 1] != t[j + 1] && j >= 0) {
            j = next[j];
        }
        if (t[i] == t[j + 1]) {
            next[i] = j + 1;
        } else {
            next[i] = 0;
        }
    }
}

void KMP(string t, string s, vector<int>& position, vector<int> next) {
    for (int i = 0; i < s.size(); i++) {
        int pos = 0;
        int j   = i;
        while (pos < t.size() && j < s.size()) {
            if (s[j] == t[pos]) {
                j++;
                pos++;
            } else {
                if (pos == 0) {
                    j++;
                } else {
                    pos = next[pos - 1] + 1;
                }
            }
        }
        if (pos == t.size() && (j - i) == t.size()) {
            position.push_back(i);
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