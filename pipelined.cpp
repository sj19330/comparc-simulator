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
                break;
            case DIV:
                val.value =  registers[line.vars[1]].value / registers[line.vars[2]].value;
                break;
            case MOD:
                val.value = int(registers[line.vars[1]].value) % int(registers[line.vars[2]].value);
                break;
            case LD:
                val.value = memory[int(registers[line.vars[1]].value) + int(registers[line.vars[2]].value)];
            case LDI:
                val.value = line.vars[1];
                break;
            case MV:
                val.value = registers[line.vars[1]].value;
                break;
            case STR:
            // add somthing to make sure the values are ints before 
                memory[int(registers[line.vars[1]].value) + int(registers[line.vars[2]].value)] = registers[line.vars[0]].value;
                val.skip = true;
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

    bool memoryAccess(executeReturn executedInstruction, Register registers[]){
        return true; // still in order so doesnt matter yet
    }

    void writeBack(executeReturn executedInstruction, Register *registers){
        registers[executedInstruction.storeReg].value = executedInstruction.value;
        registers[executedInstruction.storeReg].safe = true;
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
                safe = memoryAccess(accessInstructionMemory, registers);
                memAccHasBeenRun = true;
                cout << "Register accessed: " << accessInstructionMemory.storeReg << endl;
            }else{cout << "memAccess was not ran this cycle." << endl;}

            //WB
            if(WBInput.size() > 0){
                WBin = WBInput.front();
                WBInput.pop();
                if(!WBin.finished){
                    writeBack(WBin, registers);
                    cout << "value written back: " << WBin.value << " in register: " << WBin.storeReg << endl;
                }else{
                    cout << "Halting" << endl;
                    FINISHED = 1;
                }
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

        cout << " clock cycles: " << CLOCK << endl << " instructions executed: " << instructionsExecuted <<  endl << " Program counter: " << PC << endl << " instructions per cycle: " << ((round(float(instructionsExecuted)/float(CLOCK)*100))/100) << endl << " Pipelined" << endl <<endl;
    } 
};
