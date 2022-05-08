#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include "nonPL.cpp"
#include <queue>



using namespace std;

class Pipeline:public NonPipeline{
public:
    
    ///// gets the line from instructions corresponding to where the program counter is currently pointing
    fetchReturn fetch(string instructions[], int *PC){
        fetchReturn val;
        val.instruction = instructions[(*PC-1)];
        val.branch = checkIfBranch(val.instruction);
        *PC = *PC + 1; ///difference
        return val;

    }


    bool checkInputSafety(opline line, Register *registers){
        bool check1 = true;
        bool check2 = true;
        bool check3 = true;
        if(line.operation != BR && line.operation != HALT){
            check1 = registers[line.vars[0]].safe;
            if(line.operation != LDI){
                check2 = registers[line.vars[1]].safe;
                if(line.operation != ADDI && line.operation != SUBI && line.operation != MV && line.operation != BRLT && line.operation != BRNE && line.operation != BRE){
                    check3 = registers[line.vars[2]].safe;
                }
            }
        }
        if(check1 && check2 && check3){
            return true;
        }else{
            return false;
        }
    }


    executeReturn execute(opline line, Register *registers, float *memory, unordered_map<string, int> LABELS, int *PC, int *FINISHED, int *instructionsExecuted){
        executeReturn val;
        val.value = 0;
        val.skip = false;
        val.finished = false;
        val.branch = line.branch;
        val.extraCycles = 0;
        val.toMemory = false;
        if(!line.branch && line.operation != HALT && line.operation != STR){
            val.storeReg = line.vars[0];
            registers[line.vars[0]].safe = false;
        }
        switch(line.operation){
            case ADD:
                val.value =  registers[line.vars[1]].value + registers[line.vars[2]].value;
                break;
            case ADDI:
                val.value =  registers[line.vars[1]].value + line.vars[2];
                break;
            case SUB:
                val.value =  registers[line.vars[1]].value - registers[line.vars[2]].value;
                break;
            case SUBI:
                val.value =  registers[line.vars[1]].value - line.vars[2];
                break;
            case MUL: 
                val.value =  registers[line.vars[1]].value * registers[line.vars[2]].value;
                val.extraCycles = 2;
                break;
            case DIV:
                val.value =  registers[line.vars[1]].value / registers[line.vars[2]].value;
                val.extraCycles = 2;
                break;
            case MOD:
                val.value = int(registers[line.vars[1]].value) % int(registers[line.vars[2]].value);
                val.extraCycles = 4;
                break;
            case LD:
                val.value = memory[int(registers[line.vars[1]].value) + int(registers[line.vars[2]].value)];
                break;
            case LDI:
                val.value = line.vars[1];
                break;
            case MV:
                val.value = registers[line.vars[1]].value;
                break;
            case STR: 
                val.storeReg = int(registers[line.vars[1]].value) + int(registers[line.vars[2]].value);
                val.value = registers[line.vars[0]].value;
                val.toMemory = true;
                break;
            case BRNE:
                if(registers[line.vars[0]].value != registers[line.vars[1]].value) *PC = LABELS[line.label];
                val.skip = true;
                break;
            case BRE:
                if(registers[line.vars[0]].value == registers[line.vars[1]].value) *PC = LABELS[line.label];
                val.skip = true;
                break;
            case BRLT:
                if(!(registers[line.vars[0]].value < registers[line.vars[1]].value)) *PC = LABELS[line.label];
                val.skip = true;
                break;
            case BR: 
                *PC = LABELS[line.label];
                val.skip = true;
                break;
            case CMP: 
                if(registers[line.vars[1]].value< registers[line.vars[2]].value) val.value = -1;
                else if (registers[line.vars[1]].value == registers[line.vars[2]].value) val.value = 0;
                else if (registers[line.vars[1]].value > registers[line.vars[2]].value) val.value = 1;
                break;
            case HALT:
                val.finished = true;
            default:
                ;
        }
        *instructionsExecuted = *instructionsExecuted + 1;
        return val;
    }


    void writeBack(executeReturn executedInstruction, Register *registers, float *memory){
        if(executedInstruction.toMemory){
            memory[executedInstruction.storeReg] = executedInstruction.value;
        }else{
            registers[executedInstruction.storeReg].value = executedInstruction.value;
            registers[executedInstruction.storeReg].safe = true;
        }
    }



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

            int thisPC = PC;
            cout << endl << "PC: " << PC << endl;

            bool decodeHasBeenRun = false;
            bool fetchHasBeenRun = false;
            bool executeHasBeenRun = false;
            //maybe these can be one
            fetchReturn fRet;
            opline dRet;
            executeReturn eRet;

            if(!branch){
                //fetch
                if(thisPC <= programLength){
                    fRet = fetch(instructionMemory, &thisPC);
                    fetchHasBeenRun = true;
                    branch = fRet.branch;
                    cout << "Fetched instruction: " << fRet.instruction << endl;
                }
                else{cout << "Fetch was not ran this cycle." << endl;}
            }else{cout << "Fetch was not ran this cycle." << endl;}
            
            //decode
            if(decodeInput.size() > 0){
                fetchReturn input = decodeInput.front();
                decodeInput.pop();
                dRet = decode(input);
                decodeHasBeenRun = true;
                cout << "Decoded instruction: " << input.instruction << endl;
            }else{cout << "Decode was not ran this cycle." << endl;}
            
            //execute
            if(executeInput.size() > 0){
                opline input = executeInput.front();
                bool safe = checkInputSafety(input, registers);
                if(safe){
                    eRet = execute(input, registers, memory, LABELS, &thisPC, &FINISHED, &instructionsExecuted);
                    CLOCK = CLOCK + eRet.extraCycles;
                    executeInput.pop();
                    executeHasBeenRun = true;
                    if(eRet.branch){
                        branch = false;
                    }
                    cout << "Executed instruction: " << input.operation << endl;
                }else{cout << "NOT SAFE: Data Dependency.Execute was not ran this cycle." << endl;}
            }else{cout << "Execute was not ran this cycle." << endl;
}
            
            //WB
            if(WBInput.size() > 0){
                executeReturn input = WBInput.front();
                WBInput.pop();
                if(!input.finished){
                    writeBack(input, registers, memory);
                    cout << "value written back: " << input.value << " in register: " << input.storeReg << endl;
                }else{
                    cout << "Halting" << endl;
                    FINISHED = 1;
                }
            }else{cout << "WriteBack was not ran this cycle." << endl;}

            //update queues
            if(fetchHasBeenRun){
                decodeInput.push(fRet);
            }
            if(decodeHasBeenRun){
                executeInput.push(dRet);
            }
            if(executeHasBeenRun && !eRet.skip){
                WBInput.push(eRet);
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
        cout << " clock cycles: " << CLOCK << endl << " instructions executed: " << instructionsExecuted <<  endl << " Program counter: " << PC << endl << " instructions per cycle: " << ((round(float(instructionsExecuted)/float(CLOCK)*100))/100) << endl << " Pipelined" << endl <<endl;
        cout << memory[0] << endl;
    } 
};
