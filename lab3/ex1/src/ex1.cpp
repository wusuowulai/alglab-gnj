#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;
using namespace filesystem;
using namespace chrono;

vector<int> dis, road;

void split(string str, vector<string>& arr, string cut) {
    string::size_type pos1, pos2;
    pos2 = str.find(cut);
    pos1 = 0;
    while (string::npos != pos2) {
        arr.push_back(str.substr(pos1, pos2 - pos1));
        pos1 = pos2 + cut.size();
        pos2 = str.find(cut, pos1);
    }
    if (pos1 != str.length()) {
        arr.push_back(str.substr(pos1));
    }
}

void bellman_ford(int number, vector<vector<int>> graph) {
    dis[0] = 0;
    for (int k = 0; k < number; k++) {
        for (int i = 0; i < number; i++) {
            for (int j = 0; j < number; j++) {
                if (graph[i][j] != INT_MAX && dis[i] != INT_MAX &&
                    dis[j] > dis[i] + graph[i][j]) {
                    dis[j]  = dis[i] + graph[i][j];
                    road[j] = i;
                }
            }
        }
    }
}

void output_road(int j, int flag, ofstream& output) {
    if (j != 0) {
        output_road(road[j], 0, output);
    }
    if (flag == 0) {
        output << j << " ";
    } else {
        output << j;
    }
}

int main() {
    path         dirname = (__FILE__);
    string       input[1000], tmp;
    vector<path> filepath;
    vector<path> outputfile;
    int          order = 0, number;
    /*
    auto dir = dirname.parent_path().parent_path();
    dir += "\\input";
    for(auto const & dir_entry: filesystem::recursive_directory_iterator{dir}){
        if(!dir_entry.is_directory()){
            auto p = dir_entry.path().string();
            filepath.push_back(p);
        }
    }

    for(int i =0; i < filepath.size();i++){
        cout<<filepath[i]<<endl;
    }
    */
    // file loading
    auto dir       = dirname.parent_path().parent_path();
    auto timefile1 = dirname.parent_path().parent_path();
    for (int a = 1; a < 5; a++) {
        for (int b = 1; b < 3; b++) {
            auto p = dir;
            p += "\\input\\input" + to_string(a) + to_string(b) + ".txt";
            filepath.push_back(p);
        }
    }
    timefile1 += "\\output\\time.txt";
    for (int a = 1; a < 5; a++) {
        for (int b = 1; b < 3; b++) {
            auto p = dir;
            p += "\\output\\output" + to_string(a) + to_string(b) + ".txt";
            outputfile.push_back(p);
        }
    }
    vector<vector<int>> graph;
    ifstream            file;
    ofstream            timefile;
    ofstream            output;
    timefile.open(timefile1, ios::out);
    while (order < 8) {
        graph.clear();
        dis.clear();
        road.clear();

        output.open(outputfile[order], ios::out);
        file.open(filepath[order], ios::in);
        while (getline(file, tmp)) {
            input[number++] = tmp;
        }
        file.close();

        vector<string> arr;
        graph.resize(number);
        dis.resize(number, INT_MAX);
        road.resize(number, 0);
        dis[0] = 0;

        for (int i = 0; i < number; i++) {
            arr.clear();
            graph[i].resize(number);
            split(input[i], arr, ",");
            for (int j = 0; j < number; j++) {
                int n = atoi(arr[j].c_str());
                if (n != 0) {
                    graph[i][j] = n;
                } else if (i == j) {
                    graph[i][j] = 0;
                } else {
                    graph[i][j] = INT_MAX;
                }
            }
        }
        order++;
        steady_clock::time_point t1 = steady_clock::now();
        bellman_ford(number, graph);
        steady_clock::time_point t2   = steady_clock::now();
        auto                     time = t2 - t1;
        timefile << number << ": " << duration<double, milli>(time).count()
                 << "ms" << endl;
        for (int j = 0; j < number; j++) {
            if (dis[j] != INT_MAX) {
                output << "0"
                       << "," << j << "," << dis[j] << ";";
                output_road(j, 1, output);
                output << endl;
            }
        }
        output.close();
        number = 0;
    }
}