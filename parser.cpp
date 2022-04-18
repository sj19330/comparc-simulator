#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
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
        if(line[i] == ' ' && (word.size() > 0)){
            words.push_back(word);
            word.clear();
        }else if(line[i] != ' '){
            word.push_back(line[i]);
        }
    }
    if(word.size()>0){
        words.push_back(word);
    }
    return words;
}

///////////////////////////////     
unordered_map<string, int> loadIntoMemory(string *instructionMemory, string program){
    vector<string> instructions = readtxtFile(program);
    unordered_map<string, int> LABELS;
    int j = 0;
    for(int i = 0; i<instructions.size(); i++){
        vector<string> line = split(instructions[i]);
        if(line[0] == "Label:"){
            LABELS[line[1]] = j+1;
        }
        else{
            instructionMemory[j] = instructions[i];
            j++;
        }   
    }
    return LABELS;
}