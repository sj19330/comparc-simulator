#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include "pipelined.cpp"


using namespace std;

int main(){
    string setting = "nonPL";
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
        cout << "Please pick a valid setting." << endl;
    }
}
