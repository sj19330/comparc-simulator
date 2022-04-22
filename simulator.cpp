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
    string setting = "ooo";
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
        class GF{
        public:
            void a(int i){
                cout << i << endl;
            }    
        };
        class Father: public GF{
        public:
            void a(int i, int j){
                cout << i << " and " << j << endl;
            }    
        };
        class Child: public Father{
        public: 
            void laugh(){
                cout << "heheheh" << endl;
            }
        };

        Child c;
        c.laugh();
    }
}
