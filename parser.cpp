#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include "headers/parser.h"

using namespace std;

//////////////////////////////     Read in Text file
vector<string> readtxtFile(string input){
    vector<string> lines;
    ifstream file_(input);
    if(file_.is_open()){
        string line;
        while(getline(file_, line)){
            lines.push_back(line);
        }
        file_.close();
    }
    else{
        cout << "file didn't open"<<endl;
    }
    return lines;
}
//////////////////////////////     Split line into seperate words
vector<string> split (string line){
    vector<string> words;
    string word;
    for(int i = 0; i<line.size(); i++){
        if(line[i] == ' '){
            words.push_back(word);
            word.clear();
        }else{
            word.push_back(line[i]);
        }
    }
    words.push_back(word);
    return words;
}