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
    private:
        
    public:
        opline input;
        bool instructionInEu = false;
        int cyclesLeft = 0;

        void decrementCL(){
            cyclesLeft = cyclesLeft - 1;
        }

        void newInst(opline exIn){
            input = exIn;
            instructionInEu = true;
            if(exIn.operation == MUL || exIn.operation == DIV){
                cyclesLeft = 2;
            }else{
                cyclesLeft = 0;
            }
        }

        executeReturn exec(opline line, Register *registers, float *memory, unordered_map<string, int> LABELS, int *PC, int *FINISHED, int *instructionsExecuted){      
            executeReturn val;
            val.branch = line.branch;
            val.finished = false;
            val.skip = false;
            val.timestamp = line.timestamp;
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
            return val;
        }    };

    bool memoryAccess(executeReturn executedInstruction, int position){
        if(executedInstruction.timestamp == position + 1){
            return true;
        }else{
            return false;
        } 
    }

    void run(){

        // set up 
        Register RF[32];
        float memory[516];
        fill_n(memory, 516, 0);
        string IM[64];
        int FINISHED = 0;
        int CLOCK = 0;
        int instructionsExecuted = 0;
        string program = "machineCode.txt";
        int programLength;
        int plWidth = 4;
        ExecutionUnit EU[plWidth];
        
        
        int WBPosition = -1;
        int PC = 1;
        bool branch = false;
        int timestamp = 0;

            
        
        // loads the program into the instructionMemory and finds all the 
        // labels for branches, putting them in table: "LABELS"
        // also fills program length with the length of the program
        unordered_map<string, int> LABELS = loadIntoMemory(IM, program, &programLength);
 
        queue<fetchReturn> decodeInput;
        queue<opline> RS[plWidth];
        queue<executeReturn> memAccInput;
        queue<executeReturn> WBInput;
        
        //unpipelined loop
        while(FINISHED != 1){
        // for(int k=0; k<20; k++){

            int thisPC = PC;
            cout << endl << "PC: " << PC << endl;

            bool decodeHasBeenRun[plWidth];
            bool fetchHasBeenRun[plWidth];
            bool executeHasBeenRun[plWidth];
            bool memAccAddedToQueue[plWidth];
            for(int i=0; i<plWidth; i++){
                decodeHasBeenRun[i] = false;
                fetchHasBeenRun[i] = false;
                executeHasBeenRun[i] = false;
                memAccAddedToQueue[i] = false;
            }
            // bool WBHasBeenRun = false;
            //maybe these can be one
            fetchReturn fRet[plWidth];

            opline dRet[plWidth];

            executeReturn exRet[plWidth];

            queue<executeReturn> maRet[plWidth];
            bool safe[plWidth];
            executeReturn WBin;

            for(int i=0; i<plWidth; i++){
                if(!branch){
                    //fetch
                    if(thisPC <= programLength){
                        fRet[i] = fetch(IM, &thisPC);
                        fetchHasBeenRun[i] = true;
                        branch = fRet[i].branch;
                        cout << i << "Fetched instruction: " << fRet[i].instruction << endl;
                    }
                    else{cout << i << "Fetch was not ran this cycle." << endl;}
                }else{cout << i << "Fetch was not ran this cycle." << endl;}
            }
            
            
            //decode
            for(int i=0; i<plWidth; i++){
                if(decodeInput.size() > 0){
                    fetchReturn input = decodeInput.front();
                    decodeInput.pop();
                    dRet[i] = decode(input);
                    if(!dRet[i].branch && dRet[i].operation != STR){
                        dRet[i].timestamp = timestamp;
                        timestamp++;
                    }
                    decodeHasBeenRun[i] = true;
                    cout << i << "Decoded instruction: " << input.instruction << endl;
                }else{cout << i << "Decode was not ran this cycle." << endl;}
            }
            
            
            //execute
            for(int i=0; i<plWidth; i++){
                if(EU[i].cyclesLeft == 0){
                    if(EU[i].instructionInEu){
                        exRet[i] = EU[i].exec(EU[i].input, RF, memory, LABELS, &thisPC, &FINISHED, &instructionsExecuted);
                        EU[i].instructionInEu = false;
                        instructionsExecuted++;
                        executeHasBeenRun[i] = true;
                        if(exRet[i].branch){
                            branch = false;
                        }
                        cout << i << "Executed instruction: " << EU[i].input.operation << endl;
                    }else{
                        if(RS[i].size() > 0){
                            opline input = RS[i].front();
                            bool safe = checkInputSafety(input, RF);
                            if(safe){
                                EU[i].newInst(input);
                                RS[i].pop();
                                if(EU[i].cyclesLeft == 0){
                                    exRet[i] = EU[i].exec(EU[i].input, RF, memory, LABELS, &thisPC, &FINISHED, &instructionsExecuted);
                                    EU[i].instructionInEu = false;
                                    instructionsExecuted++;
                                    executeHasBeenRun[i] = true;
                                    if(exRet[i].branch){
                                        branch = false;
                                    }
                                    cout << i << "Executed instruction: " << EU[i].input.operation << endl;
                                }else{
                                    cout << "EU busy, " <<  EU[i].cyclesLeft << " cycles left" << endl;
                                    EU[i].decrementCL();
                                }
                            }else{
                                cout << i << "NOT SAFE: Data Dependency.Execute was not ran this cycle." << endl;
                            }
                        }else{
                            cout << i << " no instructions to execute" << endl;
                        }
                    }
                }else{
                    cout << "EU busy, " <<  EU[i].cyclesLeft << " cycles left" << endl;
                    EU[i].decrementCL();
                }
            }
            
            //memacc
            for(int i=0; i<plWidth; i++){
                if(memAccInput.size() > 0){
                    executeReturn input = memAccInput.front();
                    memAccInput.pop();
                    if(input.finished){cout << i << "Program Halting after next cycle" << endl;
                        maRet[i].push(input);
                    }else{
                        cout << i << "memAccess has added instruction('s) to WriteBack queue" << endl;
                        maRet[i].push(input);
                    }
                    memAccAddedToQueue[i] = true;
                }else{cout << i << " memAccess was not ran this cycle." << endl;}
            }

            //WB
            for(int i=0; i<plWidth; i++){
                if(WBInput.size() > 0){
                    for(int j=0; j < WBInput.size(); j++){
                        WBin = WBInput.front();
                        WBInput.pop();
                        if(!WBin.finished){
                        writeBack(WBin, RF);
                        cout << i << "value written back: " << WBin.value << " in register: " << WBin.storeReg << endl;
                        }else{
                            cout << i << "Halting" << endl;
                            FINISHED = 1;
                        }
                    }
                    
                }else{cout << i << "WriteBack was not ran this cycle." << endl;}
            }



            //update queues
            for(int i=0; i<plWidth; i++){
                if(fetchHasBeenRun[i]){
                    decodeInput.push(fRet[i]);
                }
                if(decodeHasBeenRun[i]){
                    RS[i].push(dRet[i]);
                }
                if(executeHasBeenRun[i] && !exRet[i].skip){
                    memAccInput.push(exRet[i]);
                }
                if(memAccAddedToQueue[i]){
                    for(int j=0; j<maRet[i].size(); j++){
                        WBInput.push(maRet[i].front());
                        maRet[i].pop();
                    }
                }
            }

            PC = thisPC;

            //print out
            CLOCK = CLOCK + 1;
            cout << "RF: ";
            for(int i = 0; i<32; i++){
                cout << RF[i].value << " ";
            }

            cout << endl << "Clock cycles: " << CLOCK << endl;
            cout << "PC: " << PC << endl << endl;
            
        }

        cout << " clock cycles: " << CLOCK << endl << " instructions executed: " << instructionsExecuted <<  endl << " Program counter: " << PC << endl << " instructions per cycle: " << ((round(float(instructionsExecuted)/float(CLOCK)*100))/100) << endl << " Out of order" << endl <<endl;
    } 

};
