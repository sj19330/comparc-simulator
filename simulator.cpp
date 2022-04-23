#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include "OoO.cpp"
#include <queue>
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
    else if(setting == "superscalar"){
        Superscalar simulator;
        simulator.run();
        cout << " Superscalar complete" << endl;
    }
    else if(setting == "ooo"){
        OutOfOrder simulator;
        simulator.run();
        cout << " Out of order complete" << endl;
    }
    else{
       queue<int> ints;
       ints.push(1);
       ints.push(2);
       ints.push(3);
       ints.push(4);
       queue<int> copy = ints;
       copy.pop();
       cout << ints.size() << endl;
    }
}
