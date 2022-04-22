#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include "superscalar.cpp"
#include <queue>

using namespace std;

class OutOfOrder: public Superscalar{
public:
    class ExecutionUnit{
    public: 
        bool busy;
        int cyclesLeft;
        
        void decrementCL(){
            cyclesLeft = cyclesLeft - 1;
        }

        void newInst(){
            cyclesLeft = 1;
        }

        executeReturn initialiseVal(opline line){
            executeReturn val;
            val.branch = line.branch;
            val.finished = false;
            val.skip = false;
            return val;
        }

        executeReturn exec(opline line, Register *registers, float *memory, unordered_map<string, int> LABELS, int *PC, int *FINISHED, int *instructionsExecuted){
            executeReturn val = initialiseVal(line);
            if(!line.branch && line.operation != HALT){
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
                case MOD:///
                    val.value = int(registers[line.vars[1]].value) % int(registers[line.vars[2]].value);
                    break;///this needs to go
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
                    cout << line.vars[0] << line.vars[1] << endl;
                    if(registers[line.vars[0]].value != registers[line.vars[1]].value) {
                        *PC = LABELS[line.label];
                    }val.skip = true;
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
                    val.skip = true;
                    *PC = LABELS[line.label];
                    break;
                case CMP: 
                    if(registers[line.vars[1]].value < registers[line.vars[2]].value) val.value = -1;
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
    };

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
        int pipelineWidth = 1;
        ExecutionUnit execUnit; 

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
        // for(int k=0; k<20; k++){

            int thisPC = PC;
            cout << endl << "PC: " << PC << endl;

            bool decodeHasBeenRun[pipelineWidth];
            bool fetchHasBeenRun[pipelineWidth];
            bool executeHasBeenRun[pipelineWidth];
            bool memAccHasBeenRun[pipelineWidth];
            for(int i=0; i<pipelineWidth; i++){
                decodeHasBeenRun[i] = false;
                fetchHasBeenRun[i] = false;
                executeHasBeenRun[i] = false;
                memAccHasBeenRun[i] = false;
            }
            // bool WBHasBeenRun = false;
            //maybe these can be one
            fetchReturn fetched[pipelineWidth];
            fetchReturn fetchedInstruction[pipelineWidth];
            opline instruction[pipelineWidth];
            opline instructionExecutable[pipelineWidth];
            executeReturn executedInstruction[pipelineWidth];
            executeReturn accessInstructionMemory[pipelineWidth];
            bool safe[pipelineWidth];
            executeReturn WBin;

            for(int i=0; i<pipelineWidth; i++){
                if(!branch){
                    //fetch
                    if(thisPC <= programLength){
                        fetched[i] = fetch(instructionMemory, &thisPC);
                        fetchHasBeenRun[i] = true;
                        branch = fetched[i].branch;
                        cout << i << "Fetched instruction: " << fetched[i].instruction << endl;
                    }
                    else{cout << i << "Fetch was not ran this cycle." << endl;}
                }else{cout << i << "Fetch was not ran this cycle." << endl;}
            }
            
            
            //decode
            for(int i=0; i<pipelineWidth; i++){
                if(decodeInput.size() > 0){
                    fetchedInstruction[i] = decodeInput.front();
                    decodeInput.pop();
                    instruction[i] = decode(fetchedInstruction[i]);
                    decodeHasBeenRun[i] = true;
                    cout << i << "Decoded instruction: " << fetchedInstruction[i].instruction << endl;
                }else{cout << i << "Decode was not ran this cycle." << endl;}
            }
            
            
            //execute
            // for(int i=0; i<pipelineWidth; i++){
            //     if(executeInput.size() > 0 && runExec){
            //         instructionExecutable[i] = executeInput.front();
            //         bool safe = checkInputSafety(instructionExecutable[i], registers);
            //         cout << "instructinexecv0: " << registers[instructionExecutable[i].vars[0]].safe << "instructinexecv1: " << registers[instructionExecutable[i].vars[1]].safe << endl;
            //         if(safe){
            //             if(!execUnit.busy){
            //                 execUnit.busy = true;
            //                 execUnit.newInst();
            //                 executeInput.pop();
            //             }else{cout << i << "Execution unit busy" << endl;}
            //             execUnit.decrementCL();
            //             if(execUnit.cyclesLeft == 0){
            //                 execUnit.busy = false;
            //                 executedInstruction[i] = execUnit.exec(instructionExecutable[i], registers, memory, LABELS, &thisPC, &FINISHED, &instructionsExecuted); 
            //                 executeHasBeenRun[i] = true;
            //                 cout << i << "Executed instruction: " << instructionExecutable[i].operation << endl;
            //             }
            //             // executedInstruction[i] = execute(instructionExecutable[i], registers, memory, LABELS, &thisPC, &FINISHED, &instructionsExecuted)
            //             if(executedInstruction[i].branch){
            //                 branch = false;
            //             }
            //         }else{
            //             cout << i << "NOT SAFE: Data Dependency.Execute was not ran this cycle." << endl;
            //             runExec = false;
            //         }
            //     }else{cout << i << "Execute was not ran this cycle." << endl;}
            // }

            for(int i=0; i<pipelineWidth; i++){
                if(executeInput.size() > 0){
                    instructionExecutable[i] = executeInput.front();
                    bool safe = checkInputSafety(instructionExecutable[i], registers);
                    if(safe){
                        executedInstruction[i] = execUnit.exec(instructionExecutable[i], registers, memory, LABELS, &thisPC, &FINISHED, &instructionsExecuted);
                        executeInput.pop();
                        executeHasBeenRun[i] = true;
                        if(executedInstruction[i].branch){
                            branch = false;
                        }
                        cout << i << "Executed instruction: " << instructionExecutable[i].operation << endl;
                    }else{
                        cout << i << "NOT SAFE: Data Dependency.Execute was not ran this cycle." << endl;
                    }
                }else{cout << i << "Execute was not ran this cycle." << endl;}
            }
            
            //memacc
            for(int i=0; i<pipelineWidth; i++){
                if(memAccInput.size() > 0){
                    accessInstructionMemory[i] = memAccInput.front();
                    safe[i] = memoryAccess(accessInstructionMemory[i], registers);
                    memAccHasBeenRun[i] = true;
                    if(accessInstructionMemory[i].finished){cout << i << "Program Halting after next cycle" << endl;
                        memAccInput.pop();
                    }else{
                        if(safe[i]){
                            memAccInput.pop();
                            cout << i << "Register: " << accessInstructionMemory[i].storeReg << " safe" << endl;
                        }else{
                            cout << i << "Register: " << accessInstructionMemory[i].storeReg << "not safe" << endl;                       
                        }
                    }
                }else{cout << i << "memAccess was not ran this cycle." << endl;}
            }

            //WB
            for(int i=0; i<pipelineWidth; i++){
                if(WBInput.size() > 0){
                    WBin = WBInput.front();
                    WBInput.pop();
                    if(!WBin.finished){
                    writeBack(WBin, registers);
                    cout << i << "value written back: " << WBin.value << " in register: " << WBin.storeReg << endl;
                    }else{
                        cout << i << "Halting" << endl;
                        FINISHED = 1;
                    }
                }else{cout << i << "WriteBack was not ran this cycle." << endl;}
            }



            //update queues
            for(int i=0; i<pipelineWidth; i++){
                if(fetchHasBeenRun[i]){
                    decodeInput.push(fetched[i]);
                }
                if(decodeHasBeenRun[i]){
                    executeInput.push(instruction[i]);
                }
                // cout << !executedInstruction[i].skip << endl;
                if(executeHasBeenRun[i] && !executedInstruction[i].skip){
                    memAccInput.push(executedInstruction[i]);
                }
                if(memAccHasBeenRun[i] && safe[i]){
                    WBInput.push(accessInstructionMemory[i]);
                }
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
            cout << "FINISHED: " << FINISHED << endl << endl;
            
        }

        cout << " clock cycles: " << CLOCK << endl << " instructions executed: " << instructionsExecuted <<  endl << " Program counter: " << PC << endl << " instructions per cycle: " << ((round(float(instructionsExecuted)/float(CLOCK)*100))/100) << endl << " OoO complete" << endl <<endl;
    }
};