#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include "pipelined.cpp"
#include <queue>



using namespace std;

class Superscalar: public Pipeline{
public:
    void run(){

        // set up 
        Register registers[32];
        float memory[516];
        fill_n(memory, 516, 0);
        string instructionMemory[64];
        int FINISHED = 0;
        int CLOCK = 0;
        int instructionsExecuted = 0;
        string program = "machineCode.txt";
        int programLength;
        int bubbles;

        int PC = 1;
        bool branch = false;
            
        
        // loads the program into the instructionMemory and finds all the 
        // labels for branches, putting them in table: "LABELS"
        // also fills program length with the length of the program
        unordered_map<string, int> LABELS = loadIntoMemory(instructionMemory, program, &programLength);
 
        queue<fetchReturn> decodeInput;
        queue<opline> executeInput;
        queue<executeReturn> memAccInput;
        queue<executeReturn> WBInput;
        
        //unpipelined loop
        while(FINISHED != 1){

            int thisPC = PC;
            cout << endl << "PC: " << PC << endl;

            bool decodeHasBeenRun = false;
            bool fetchHasBeenRun = false;
            bool executeHasBeenRun = false;
            bool memAccHasBeenRun = false;
            // bool WBHasBeenRun = false;
            //maybe these can be one
            fetchReturn fetched;
            fetchReturn fetchedInstruction;
            opline instruction;
            opline instructionExecutable;
            executeReturn executedInstruction;
            executeReturn accessInstructionMemory;
            bool safe;
            executeReturn WBin; 

            if(!branch){
                //fetch
                if(thisPC <= programLength){
                    fetched = fetch(instructionMemory, &thisPC);
                    fetchHasBeenRun = true;
                    branch = fetched.branch;
                    cout << "Fetched instruction: " << fetched.instruction << endl;
                }
                else{cout << "Fetch was not ran this cycle." << endl;}
            }else{cout << "Fetch was not ran this cycle." << endl;}
            
            //decode
            if(decodeInput.size() > 0){
                fetchedInstruction = decodeInput.front();
                decodeInput.pop();
                instruction = decode(fetchedInstruction);
                decodeHasBeenRun = true;
                cout << "Decoded instruction: " << fetchedInstruction.instruction << endl;
            }else{cout << "Decode was not ran this cycle." << endl;}
            
            //execute
            if(executeInput.size() > 0){
                instructionExecutable = executeInput.front();
                bool safe = checkInputSafety(instructionExecutable, registers);
                if(safe){
                    executedInstruction = execute(instructionExecutable, registers, memory, LABELS, &thisPC, &FINISHED, &instructionsExecuted);
                    executeInput.pop();
                    executeHasBeenRun = true;
                    if(executedInstruction.branch){
                        branch = false;
                    }
                    cout << "Executed instruction: " << instructionExecutable.operation << endl;
                }else{cout << "NOT SAFE: Data Dependency.Execute was not ran this cycle." << endl;}
            }else{cout << "Execute was not ran this cycle." << endl;
}
            
            //memacc
            if(memAccInput.size() > 0){
                accessInstructionMemory = memAccInput.front();
                safe = memoryAccess(accessInstructionMemory, registers, &FINISHED);
                memAccHasBeenRun = true;
                if(FINISHED == 1){cout << "Program Halting after this cycle" << endl;
                }else{cout << "Register accessed: " << accessInstructionMemory.storeReg << endl;}
            }else{cout << "memAccess was not ran this cycle." << endl;}

            //WB
            if(WBInput.size() > 0){
                WBin = WBInput.front();
                WBInput.pop();
                writeBack(WBin, registers);
                // WBHasBeenRun = true;
                cout << "value written back: " << WBin.value << " in register: " << WBin.storeReg << endl;
            }else{cout << "WriteBack was not ran this cycle." << endl;}



            //update queues
            if(fetchHasBeenRun){
                decodeInput.push(fetched);
            }
            if(decodeHasBeenRun){
                executeInput.push(instruction);
            }
            if(executeHasBeenRun){
                if(!executedInstruction.skip){memAccInput.push(executedInstruction);}
            }
            if(memAccHasBeenRun && safe){
                memAccInput.pop();
                WBInput.push(accessInstructionMemory);
            }

            PC = thisPC;

            //print out
            CLOCK = CLOCK + 1;
            cout << "Registers: ";
            for(int i = 0; i<32; i++){
                cout << registers[i].value << " ";
            }

            cout << endl << "Clock cycles: " << CLOCK << endl;
            cout << "PC: " << PC << endl << endl;
            
        }

        cout << " clock cycles: " << CLOCK << endl << " instructions executed: " << instructionsExecuted <<  endl << " Program counter: " << PC << endl << " instructions per cycle: " << ((round(float(instructionsExecuted)/float(CLOCK)*100))/100) << endl << " Superscalar" << endl <<endl;
    } 

};
