#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include "nonPL.cpp"



using namespace std;

class Pipeline:public NonPipeline{
public:
    

    void run(){

        // set up 
        float registers[32];
        fill_n(registers, 32,0);
        float memory[516];
        fill_n(memory, 516, 0);
        string instructionMemory[64];
        int FINISHED = 0;
        int CLOCK = 0;
        int instructionsExecuted = 0;
        int PC = 1;
        string program = "machineCode.txt";
        bool branch = false;
            
        
        // loads the program into the instructionMemory and finds all the 
        // labels for branches, putting them in table: "LABELS"
        unordered_map<string, int> LABELS = loadIntoMemory(instructionMemory, program);
        
        
        //unpipelined loop
        while(FINISHED != 1){
            fetchReturn fetched;
            if(!branch){
                fetched = fetch(instructionMemory, &PC);
                branch = fetched.branch;
            }
            
            opline instruction = decode(fetched);
            execute(instruction, registers, memory, LABELS, &PC, &FINISHED);   
            instructionsExecuted++;

            //print out
            CLOCK = CLOCK + 3;
            cout << "registers: ";
            for(int i = 0; i<32; i++){
                cout << registers[i] << " ";
            } 
            cout << endl << "PC: " << PC << endl << endl;
        }
        cout << " clock cycles: " << CLOCK << endl << " instructions executed: " << instructionsExecuted <<  endl << " Program counter: " << PC << endl << " instructions per cycle: " << (float(instructionsExecuted)/float(CLOCK)) << endl << " Pipelined" << endl <<endl;
    } 
};
