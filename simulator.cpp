#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include "pipelined.cpp"
#include <queue>
using namespace std;

int main(){
    string setting = "pipelined";
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
    else{
    }
}
