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
        int pipelineWidth = 4;
        int fullPipeline = 0;
        int PC = 1;
        bool branch = false;
            
        
        // loads the program into the instructionMemory and finds all the 
        // labels for branches, putting them in table: "LABELS"
        // also fills program length with the length of the program
        unordered_map<string, int> LABELS = loadIntoMemory(instructionMemory, program, &programLength);
 
        queue<fetchReturn> decodeInput;
        queue<opline> executeInput;
        queue<executeReturn> WBInput;
        
        //unpipelined loop
        while(FINISHED != 1){
        // for(int k=0; k<10; k++){
            bool allExecuteRanThisCycle = true;
            int thisPC = PC;
            cout << endl << "PC: " << PC << endl;

            bool decodeHasBeenRun[pipelineWidth];
            bool fetchHasBeenRun[pipelineWidth];
            bool executeHasBeenRun[pipelineWidth];
            for(int i=0; i<pipelineWidth; i++){
                decodeHasBeenRun[i] = false;
                fetchHasBeenRun[i] = false;
                executeHasBeenRun[i] = false;
            }
            // bool WBHasBeenRun = false;
            //maybe these can be one
            fetchReturn fRet[pipelineWidth];
            opline dRet[pipelineWidth];
            executeReturn eRet[pipelineWidth];

            for(int i=0; i<pipelineWidth; i++){
                if(!branch){
                    //fetch
                    if(thisPC <= programLength){
                        fRet[i] = fetch(instructionMemory, &thisPC);
                        fetchHasBeenRun[i] = true;
                        branch = fRet[i].branch;
                        cout << i << "Fetched instruction: " << fRet[i].instruction << endl;
                    }
                    else{cout << i << "Fetch was not ran this cycle." << endl;}
                }else{cout << i << "Fetch was not ran this cycle." << endl;}
            }
            
            
            //decode
            for(int i=0; i<pipelineWidth; i++){
                if(decodeInput.size() > 0){
                    fetchReturn input = decodeInput.front();
                    decodeInput.pop();
                    dRet[i] = decode(input);
                    decodeHasBeenRun[i] = true;
                    cout << i << "Decoded instruction: " << input.instruction << endl;
                }else{cout << i << "Decode was not ran this cycle." << endl;}
            }
            
            
            //execute
            for(int i=0; i<pipelineWidth; i++){
                if(executeInput.size() > 0){
                    opline input = executeInput.front();
                    bool safe = checkInputSafety(input, registers);
                    if(safe){
                        eRet[i] = execute(input, registers, memory, LABELS, &thisPC, &FINISHED, &instructionsExecuted);
                        executeInput.pop();
                        executeHasBeenRun[i] = true;
                        if(eRet[i].branch){
                            branch = false;
                        }
                        cout << i << "Executed instruction: " << input.operation << endl;
                    }else{
                        cout << i << "NOT SAFE: Data Dependency.Execute was not ran this cycle." << endl;
                    }
                }else{cout << i << "Execute was not ran this cycle." << endl;}
            }

            //WB
            for(int i=0; i<pipelineWidth; i++){
                if(WBInput.size() > 0){
                    executeReturn input = WBInput.front();
                    WBInput.pop();
                    if(!input.finished){
                    writeBack(input, registers, memory);
                    cout << i << "value written back: " << input.value << " in register: " << input.storeReg << endl;
                    }else{
                        cout << i << "Halting" << endl;
                        FINISHED = 1;
                    }
                }else{cout << i << "WriteBack was not ran this cycle." << endl;}
            }



            //update queues
            int extraCycles = 0;
            for(int i=0; i<pipelineWidth; i++){
                if(fetchHasBeenRun[i]){
                    decodeInput.push(fRet[i]);
                }
                if(decodeHasBeenRun[i]){
                    executeInput.push(dRet[i]);
                }
                if(executeHasBeenRun[i] && !eRet[i].skip){
                    WBInput.push(eRet[i]);
                }
                if(executeHasBeenRun[i] && (eRet[i].extraCycles > extraCycles)){
                    extraCycles = eRet[i].extraCycles;                    
                }
                if(!executeHasBeenRun[i]){
                    allExecuteRanThisCycle = false;
                }
            }

            if(allExecuteRanThisCycle){
                fullPipeline = fullPipeline + 1;
            }

            PC = thisPC;

            //print out
            CLOCK = CLOCK + 1 + extraCycles;
            cout << "Registers: ";
            for(int i = 0; i<32; i++){
                cout << registers[i].value << " ";
            }

            cout << endl << "Clock cycles: " << CLOCK << endl;
            cout << "PC: " << PC << endl << endl;
            
        }

        cout << " clock cycles: " << CLOCK << endl << " instructions executed: " << instructionsExecuted <<  endl << " Program counter: " << PC << endl << " instructions per cycle: " << ((round(float(instructionsExecuted)/float(CLOCK)*100))/100) << endl << " Superscalar" << endl <<endl;
        cout << fullPipeline << endl;
        cout << memory[0] << endl; 
    } 

};
