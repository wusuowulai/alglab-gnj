#include <array>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;
using namespace chrono;
using namespace filesystem;

auto getpow(int d, int n, int q) {
    int h = 1;
    while (n) {
        if (n & 1) {
            h = h * d % q;
        }
        d = d * d % q;
        n >>= 1;
    }
    return h;
}

int rabin_karp(string& T, string& P, int d, int q, vector<int>& match) {
    int n = T.length();
    int m = P.length();
    int h = getpow(d, m - 1, q);
    int p = 0, t = 0;
    for (int i = 0; i < m; i++) {
        p = (d * p + P[i]) % q;
        t = (d * t + T[i]) % q;
    }
    int fake_match = 0;
    for (int s = 0; s <= n - m; s++) {
        if (p == t) {
            if (P == T.substr(s, m)) {
                match.push_back(s);
            } else {
                fake_match++;
            }
        }
        if (s < n - m) {
            t = (d * (t + q - T[s] * h % q) + T[s + m]) % q;
        }
    }
    return fake_match;
}

int main() {
    path p          = __FILE__;
    auto inputfile  = p.parent_path().parent_path();
    auto timefile   = p.parent_path().parent_path();
    auto resultfile = p.parent_path().parent_path();
    timefile += "\\output\\time.txt";
    resultfile += "\\output\\result.txt";
    inputfile += "\\input\\4_2_input.txt";
    constexpr int      num_nm = 5, num_dq = 4;
    constexpr array<int, num_nm> n{0x100, 0x200, 0x400, 0x800, 0x1000};
    constexpr array<int, num_nm> m{0x8, 0x10, 0x20, 0x40, 0x80};
    constexpr array<int, num_dq> d{2, 2, 10, 10};
    constexpr array<int, num_dq> q{13, 1009, 13, 1009};
    array<array<long, num_nm>, num_dq> times;
    ifstream input(inputfile);
    ofstream output(resultfile);

    for (int nm = 0; nm!=num_nm;nm++){
        string T, P;
        input >> P>>T;
        array<vector<int>, num_dq> matchs;
        array<int, num_dq> fake_matchs;
        for (int dq = 0; dq != num_dq; ++dq) {
            vector<int>match;
            auto start = high_resolution_clock::now();
            auto fake_match = rabin_karp(T, P, d[dq], q[dq], match);
            auto end = high_resolution_clock::now();
            times[dq][nm] = duration_cast<microseconds>(end - start).count();
            matchs[dq] = move(match);
            fake_matchs[dq] = fake_match;
        }
        output << matchs[0].size() << "\n";
        for (const auto i : fake_matchs) {
            output << i << " ";
        }
        output << "\n";
        for (const auto i : matchs[0]) {
            output << i + 1 << " ";
        }
        output << "\n\n";
    }
    output.close();

    ofstream time(timefile);
    for (size_t dq = 0; dq != num_dq; ++dq) {
        time << "(" << d[dq] << ", " << q[dq] << ")\n";
        for (size_t nm = 0; nm != num_nm; ++nm) {
            time << times[dq][nm] << "us ";
        }
        time << "\n";
    }
    time.close();
}