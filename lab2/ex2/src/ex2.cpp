#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;
using namespace chrono;
using namespace filesystem;

class DisjSet {
  private:
    vector<int> parent;
    vector<int> rank;  // 秩

  public:
    DisjSet(int max_size)
        : parent(vector<int>(max_size)), rank(vector<int>(max_size, 0)) {
        for (int i = 0; i < max_size; ++i)
            parent[i] = i;
    }
    // 查找根节点
    int find(int x) {
        return x == parent[x] ? x : (parent[x] = find(parent[x]));
    }
    //合并 同时进行路径优化和按秩合并
    void to_union(int x1, int x2) {
        int f1 = find(x1);
        int f2 = find(x2);
        if (rank[f1] > rank[f2])
            parent[f2] = f1;
        else {
            parent[f1] = f2;
            if (rank[f1] == rank[f2])
                ++rank[f2];
        }
    }
    bool is_same(int e1, int e2) {
        return find(e1) == find(e2);
    }
};

int BuildSet(int number, int size, vector<int> input) {
    DisjSet* set;
    set         = new DisjSet(size);
    int number1 = 0;
    for (int i = 1; i < size; i++) {
        for (int j = 0; j < i; j++) {
            number1 = number + size * i + j;
            if (input[number1] == 1) {
                set->to_union(i, j);
            }
        }
    }
    vector<int> vec;
    for (int i = 0; i < size; i++) {
        int value = set->find(i);
        if (find(vec.begin(), vec.end(), value) == vec.end()) {
            vec.push_back(value);
        }
    }
    return vec.size();
}

int main() {
    path p0(__FILE__);
    auto p1 = p0.parent_path().parent_path();
    auto p2 = p0.parent_path().parent_path();
    auto p3 = p0.parent_path().parent_path();
    p1 +="\\input\\2_2_input.txt";
    p2 +="\\output\\result.txt";
    p3 +="\\output\\time.txt";
    cout << "Loading File from " << p1 << endl;
    ifstream file;
    file.open(p1, ios::in);
    vector<int> N = {10, 15, 20, 25, 30};
    vector<int> input;
    int         number = 0;
    while (!file.eof()) {
        file >> number;
        input.push_back(number);
    }
    file.close();
    number = 0;
    ofstream outfile, timefile;
    outfile.open(p2, ios::out);
    timefile.open(p3, ios::out);
    for(int i=0;i<N.size();i++){
        steady_clock::time_point t1 = steady_clock::now();
        outfile<<BuildSet(number, N[i], input)<<endl;
        steady_clock::time_point t2    = steady_clock::now();
        auto                     time = t2 - t1;
        timefile << duration<double, micro>(time).count() << "us" << endl;
        number +=N[i]*N[i];
    }
}