#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include "pipelined.cpp"


using namespace std;

int main(){
    string setting = "pipelined";
    if(setting == "nonPL"){
        NonPipeline simulator;
        simulator.run();
        cout << "done" << endl;
    }
    else if(setting == "pipelined"){
        Pipeline simulator;
        simulator.run();
        cout << "yes boiiisssss" << endl;
    }
    else{
        cout << "Please pick a valid setting." << endl;
    }
}
