#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include "superscalar.cpp"
#include <queue>
using namespace std;

int main(){
    string setting = "superscalar";
    if(setting == "nonPL"){
        NonPipeline simulator;
        simulator.run();
        cout << " NonPL complete" << endl;
    }
    else if(setting == "pipelined"){
        Pipeline simulator;
        simulator.run();
        cout << " Pipelined complete" << endl;
    }
    else if(setting == "superscalar"){
        Superscalar simulator;
        simulator.run();
        cout << " Superscalar complete" << endl;
    }
    else{
        
    }
}
