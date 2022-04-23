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
        executeReturn initialiseVal(opline line){
                executeReturn val;
                val.branch = line.branch;
                val.finished = false;
                val.skip = false;
                return val;
            }
    public: 
        bool busy = false;
        int cyclesLeft;
        
        void decrementCL(){
            cyclesLeft = cyclesLeft - 1;
        }

        void newInst(opline exIn){
            if(exIn.operation == MUL || exIn.operation == DIV){
                cyclesLeft = 2;
            }else{
                cyclesLeft = 0;
            }
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
        }
    };

    bool memoryAccess(executeReturn executedInstruction, int position){
        if(executedInstruction.timestamp == position + 1){
            return true;
        }else{
            return false;
        } 
    }

    executeReturn getNext(vector<executeReturn> values, int WBposition){
        executeReturn val;
        val.timestamp = WBposition -1;
        for(int i=0; i<values.size(); i++){
            if(values[i].timestamp == WBposition + 1){
                val = values[i];
            }
        }
        return val;
    }

    queue<executeReturn> findnext(queue<executeReturn> maStore, int WBposition){
        queue<executeReturn> copy = maStore;
        vector<executeReturn> vcopy(copy.size());
        queue<executeReturn> returnVal;
        bool maybeAnother = true;
        for(int i=0; i<copy.size(); i++){
            vcopy[i] = copy.front();
            copy.pop();
        }
        while(maybeAnother){
            executeReturn next = getNext(vcopy, WBposition);
            if(next.timestamp < WBposition){
                maybeAnother = false;
            }else{
                returnVal.push(next);
                WBposition++;
            }
        }
        return returnVal;
    }

    // queue<executeReturn> newMaQueue(queue<executeReturn> others, queue<executeReturn>maStore){
    //     queue<executeReturn> othersCopy = others;
    //     queue<executeReturn> returnQueue;
    //     for(int i=0; i < others.size(); i++){

    //     }
    //     queue<executeReturn> maStoreCopy = maStore;
    //     return returnQueue;
    // }

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
        int plWidth = 1;
        ExecutionUnit execUnit[plWidth]; 
        int WBposition = -1;

        int PC = 1;
        bool branch = false;
        int timestamp = 0;
            
        
        // loads the program into the IM and finds all the 
        // labels for branches, putting them in table: "LABELS"
        // also fills program length with the length of the program
        unordered_map<string, int> LABELS = loadIntoMemory(IM, program, &programLength);
 
        //queues for each stage of pipeline
        queue<fetchReturn> decodeInput;
        queue<opline> issueInput;
        queue<opline> RS[plWidth];
        queue<executeReturn> memAccInput;
        queue<executeReturn> wbInput;
        executeReturn exStore[plWidth];
        
        //unpipelined loop
        while(FINISHED != 1){
        // for(int k=0; k<20; k++){
            
            int thisPC = PC;
            cout << endl << "PC: " << PC << endl;

            bool decodeHasBeenRun[plWidth];
            bool fetchHasBeenRun[plWidth];
            bool executeHasBeenRun[plWidth];
            bool memAccHasBeenRun[plWidth];
            for(int i=0; i<plWidth; i++){
                decodeHasBeenRun[i] = false;
                fetchHasBeenRun[i] = false;
                executeHasBeenRun[i] = false;
                memAccHasBeenRun[i] = false;
            }
            // bool WBHasBeenRun = false;
            //maybe these can be one
            fetchReturn fRet[plWidth];//returnd from fetch and updated at end of cycle

            opline dRet[plWidth];
            
            executeReturn exRet[plWidth];
            
            executeReturn maIn[plWidth];
            queue<executeReturn> maStore;
            queue<executeReturn> maRet[plWidth];
            
            executeReturn wbIn;

            for(int i=0; i<plWidth; i++){
                if(!branch){ // branches pause fetch
                    //fetch
                    if(thisPC <= programLength){
                        fRet[i] = fetch(IM, &thisPC);// fetch instruction as a string from IM
                        fetchHasBeenRun[i] = true;// make known that fetch has been run for later
                        branch = fRet[i].branch; // set wether instruction is a branch or not 
                        cout << i << "Fetched instruction: " << fRet[i].instruction << "  branch?: " << branch << endl;
                    }
                    else{cout << i << "Fetch was not ran this cycle." << endl;}
                }else{cout << i << "Fetch was not ran this cycle due to branch." << endl;}
            }
            
            
            //decode
            for(int i=0; i<plWidth; i++){
                if(decodeInput.size() > 0){ // if there are instructions waiting to be decoded
                    fetchReturn input = decodeInput.front(); // get the first element in the queue
                    decodeInput.pop(); // pop off the front of the input queue
                    dRet[i] = decode(input); // get the opcode from the next instruction in the queue
                    if(!dRet[i].branch && dRet[i].operation != STR){ // if the instruction needs to be written back to a register, add a timestam to it so WB knows the original order
                        cout << "time stamped"  << timestamp << endl;
                        dRet[i].timestamp = timestamp;
                        timestamp++;
                    }
                    decodeHasBeenRun[i] = true; // make known decode has been run 
                    cout << i << "Decoded instruction: " << input.instruction << endl;
                }else{cout << i << "Decode was not ran this cycle." << endl;}
            }

            for(int i=0; i<plWidth; i++){
                if(RS[i].size() > 0){
                    opline input = RS[i].front();
                    bool safe = checkInputSafety(input, RF); ///gets input for execute unit from queue
                    if(safe){ // if there is no data dependencies 
                        if(execUnit[i].busy){
                            cout << i << "Execution unit busy, " << execUnit[i].cyclesLeft << " cycles left" << endl;
                        }else{
                            execUnit[i].newInst(input);
                            execUnit[i].busy = true;
                            exStore[i] = execUnit[i].exec(input, RF, memory, LABELS, &thisPC, &FINISHED, &instructionsExecuted);
                            exStore[i].timestamp = input.timestamp;//shouldnt be input should be executing instructon
                            instructionsExecuted = instructionsExecuted + 1;
                            RS[i].pop();
                        }
                        if(execUnit[i].cyclesLeft == 0){
                            exRet[i] = exStore[i];
                            executeHasBeenRun[i] = true;
                            execUnit[i].busy = false;
                            cout << i << "Executed instruction: " << input.operation << endl;
                        }else{
                            execUnit[i].decrementCL();
                            cout << "execution in process " << endl;
                        }
                        if(exRet[i].branch){
                            branch = false;
                        }

                    }else{
                        cout << i << "NOT SAFE: Data Dependency.Execute was not ran this cycle." << endl;
                    }
                }else{cout << i << "Execute was not ran this cycle." << endl;}
            }

            
            //memacc
            for(int i=0; i<plWidth; i++){
                if(memAccInput.size() > 0){
                    executeReturn input = memAccInput.front();
                    cout << "ts: " << input.timestamp << "store reg " << input.storeReg << endl;
                    memAccInput.pop();  
                    bool nextWB = memoryAccess(input, WBposition);
                    if(nextWB){
                        queue<executeReturn> nextWBs;
                        nextWBs.push(input);
                        WBposition = WBposition + 1;
                        //add to queue
                        if(maStore.size()>0){
                            queue<executeReturn> others = findnext(maStore, WBposition); //function that searches through buffer and returns an array of consecutive things which are in order 
                            // maStore = newMaQueue(others, maStore);
                            WBposition = WBposition + others.size();// set WBposition to new position
                            for(int i=0; i<others.size(); i++){ 
                                nextWBs.push(others.front());// add all in that array returned to the queue being sent to WB
                                others.pop();
                            }
                            // maStore = others[1];
                        }
                        maRet[i] = nextWBs;
                        memAccHasBeenRun[i] = true;
                        cout << "memAccess has added instruction('s) to WriteBack queue" << endl;
                    }
                    else{
                        // add to buffer
                        maStore.push(input);
                        cout << "memAccess is waiting for correct order" << endl;
                    }
                }else{cout << i << "memAccess was not ran this cycle." << endl;}
            }

            //WB
            for(int i=0; i<plWidth; i++){
                if(wbInput.size() > 0){
                    for(int i=0; i<wbInput.size(); i++){
                        wbIn = wbInput.front();
                        wbInput.pop();
                        if(!wbIn.finished){
                            writeBack(wbIn, RF);
                            cout << wbIn.timestamp << endl;
                            cout << i << "value written back: " << wbIn.value << " in register: " << wbIn.storeReg << endl;
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
                // cout << !exRet[i].skip << endl;
                if(executeHasBeenRun[i] && !exRet[i].skip){
                    memAccInput.push(exRet[i]);
                }
                if(memAccHasBeenRun[i]){
                    for(int j=0; i<maRet[i].size(); j++){
                        wbInput.push(maRet[i].front());
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

        cout << " clock cycles: " << CLOCK << endl << " instructions executed: " << instructionsExecuted <<  endl << " Program counter: " << PC << endl << " instructions per cycle: " << ((round(float(instructionsExecuted)/float(CLOCK)*100))/100) << endl << " OoO complete" << endl <<endl;
    }
};