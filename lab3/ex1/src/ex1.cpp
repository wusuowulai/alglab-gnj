#include<iostream>
#include<filesystem>
#include<vector>
#include<chrono>
#include<fstream>
#include<string>

using namespace std;
using namespace filesystem;

void split(string str, vector<string> & arr, string cut){
    string::size_type pos1, pos2;
    pos2 = str.find(cut);
    pos1 = 0;
    while(string::npos !=pos2){
        arr.push_back(str.substr(pos1, pos2 - pos1));
        pos1 = pos2 + cut.size();
        pos2 = str.find(cut, pos1);
    }
    if(pos1 !=str.length()){
        arr.push_back(str.substr(pos1));
    }
}




int main(){
    path dirname = (__FILE__);
    string input[1000], tmp;
    auto file1 = dirname.parent_path().parent_path();
    file1 += "\\input\\input11.txt";
    ifstream file;
    file.open(file1, ios::in);
    int number = 0;
    while(1){
        while(getline(file,tmp)){
            cout<<tmp<<endl;
            input[number++] = tmp;
        }
        file.close();
        vector<string> arr;
        split(input[0], arr, ",");
        for(auto i = 0; i != arr.size();i++){
            int n = atoi(arr[i].c_str());
            cout<<n<<" ";
        }
        cout<<endl;





        number = 0;
        break;
    }
}