#include <chrono>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#define MAX_N 1000

using namespace std;
using namespace filesystem;
using namespace chrono;

int graph[MAX_N][MAX_N];     //邻接矩阵存储图 创造源点标号为0
int graphnew[MAX_N][MAX_N];  //邻接矩阵存储图 修改代价
int dis[MAX_N];              //存储所有顶点到源点的距离
int cost[MAX_N][MAX_N];      //存储最终最短路径的长度

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

void bellman_ford(int number) {
    dis[0] = 0;
    for (int k = 0; k < number; k++) {
        for (int i = 0; i < number; i++) {
            for (int j = 0; j < number; j++) {
                if (graph[i][j] != INT_MAX && dis[i] != INT_MAX &&
                    dis[j] > dis[i] + graph[i][j]) {
                    dis[j] = dis[i] + graph[i][j];
                }
            }
        }
    }
}

void dijkstra(int number, int i) {
    vector<int> dijkstradis;
    dijkstradis.resize(number + 1);
    for (int j = 1; j <= number; j++) {
        dijkstradis[j] = graphnew[i][j];
        cost[i][j]     = graph[i][j];
    }

    vector<int> if_sure;
    if_sure.resize(number + 1, 0);
    if_sure[i] = 1;

    int min, u;

    for (int j = 1; j < number; j++) {
        min = INT_MAX;
        for (int k = 1; k <= number; k++) {
            // 找到ij_dis中未标记点中的最小值
            if (if_sure[k] == 0 && dijkstradis[k] < min) {
                min = dijkstradis[k];
                u   = k;
            }
        }
        if_sure[u] = 1;
        for (int m = 1; m <= number; m++) {
            //更新dijkstradis
            if (graph[u][m] < INT_MAX) {
                if (dijkstradis[m] > dijkstradis[u] + graphnew[u][m]) {
                    dijkstradis[m] = dijkstradis[u] + graphnew[u][m];
                    cost[i][m]     = cost[i][u] + graph[u][m];
                }
            }
        }
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
    ifstream file;
    ofstream timefile;
    ofstream output;
    timefile.open(timefile1, ios::out);

    while (order < 8) {
        memset(graph, 0, sizeof(graph));     //初始化graph
        memset(graphnew, 0, sizeof(graph));  //初始化graph_new
        output.open(outputfile[order], ios::out);
        file.open(filepath[order], ios::in);
        while (getline(file, tmp)) {
            input[number++] = tmp;
        }
        file.close();

        vector<string> arr;
        for (int i = 0; i <= number; i++) {
            for (int j = 0; j <= number; j++) {
                if (i == j)
                    cost[i][j] = 0;
                else
                    cost[i][j] = INT_MAX;
            }
        }
        for (int i = 0; i <= number; i++) {
            dis[i] = INT_MAX;
        }
        for (int i = 0; i < number; i++) {
            arr.clear();
            split(input[i], arr, ",");
            for (int j = 0; j < number; j++) {
                int n = atoi(arr[j].c_str());
                if (n != 0) {
                    graph[i + 1][j + 1] = n;
                } else if (i == j) {
                    graph[i + 1][j + 1] = 0;
                } else {
                    graph[i + 1][j + 1] = INT_MAX;
                }
            }
        }
        order++;

        for (int j = 0; j <= number; j++) {
            graph[0][j] = 0;
            graph[j][0] = INT_MAX;
        }

        steady_clock::time_point t1 = steady_clock::now();
        bellman_ford(number + 1);
        for (int i = 1; i <= number; i++)
            for (int j = 1; j <= number; j++) {
                if (graph[i][j] != INT_MAX)
                    graphnew[i][j] = graph[i][j] + dis[i] - dis[j];
                else
                    graphnew[i][j] = graph[i][j];
            }
        for (int i = 1; i <= number; i++) {
            dijkstra(number, i);
        }
        steady_clock::time_point t2   = steady_clock::now();
        auto                     time = t2 - t1;
        timefile << number << ": " << duration<double, milli>(time).count()
                 << "ms" << endl;

        for (int i = 1; i <= number; i++) {
            for (int j = 1; j <= number; j++) {
                if (cost[i][j] != INT_MAX) {
                    output << cost[i][j] << " ";
                } else {
                    output << "X ";
                }
            }
            output << endl;
        }
        cout << number << endl;
        output.close();
        number = 0;
    }
}