#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include "headers/convert.h"
#include "headers/parser.h"


using namespace std;

class NonPipeline{
public:

    struct Register{
        float value;
        bool safe;
        Register(){
            value = 0;
            safe = true;
        }
    }typedef Register;

    struct fetchReturn{
        string instruction;
        bool branch;
    } typedef fetchReturn;

    struct opline {
        op operation;
        vector<int> vars;
        string label;
        bool branch;
        int instructionLength;
        int timestamp;
    } typedef opline;

    struct executeReturn{
        float value;
        int storeReg;
        bool skip;
        bool branch;
        bool finished;
        int timestamp;
    };

    //////////////////////////////      Stages

    ///// fetch simulator
    ///// helper function that checks if an instruction being fetched is a branch or not
    bool checkIfBranch(string instruction){
        bool branch = false;
        vector<string> check = split(instruction);
        if(check[0] == "Br"||check[0] == "Bre"||check[0] == "Brne"||check[0] == "Brlt"){
            branch = true;
        }
        return branch;
    }

    ///// gets the line from instructions corresponding to where the program counter is currently pointing
    fetchReturn fetch(string instructions[], int *PC){
        fetchReturn val;
        val.instruction = instructions[(*PC-1)];
        val.branch = checkIfBranch(val.instruction);
        cout << val.instruction  << endl;
        return val;

    }

    ///// decode simulator
    ///// splits the line into an opcode and variables for the computer to execute
    opline decode(fetchReturn fetchedInstruction){
        opline opline;
        vector<int> decodedElem;
        vector<string> elements = split(fetchedInstruction.instruction);

        opline.operation = decodeOp(elements[0]);
        if(fetchedInstruction.branch){
            opline.label = elements[1];
            elements.erase(elements.begin());
            opline.branch = true;
        }
        else{
            opline.branch = false;
        }
        elements.erase(elements.begin());
        for(int i = 0; i < elements.size(); i++){
            int elem = decodeReg(elements[i]);
            decodedElem.push_back(elem);
        }
        opline.vars = decodedElem;
        return opline; 
    }

    ///// execute simulator 
    ///// takes the opcode and vars and executes accordingly
    executeReturn execute(opline line, Register *registers, float *memory, unordered_map<string, int> LABELS, int *PC, int *FINISHED, int *instructionsExecuted){
        executeReturn val;
        if(!line.branch && line.operation != HALT){
            val.storeReg = line.vars[0];
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
                if(registers[line.vars[0]].value != registers[line.vars[1]].value) *PC = LABELS[line.label]-1;
                val.skip = true;
                break;
            case BRE:
                if(registers[line.vars[0]].value == registers[line.vars[1]].value) *PC = LABELS[line.label]-1;
                val.skip = true;
                break;
            case BRLT:
                if(!(registers[line.vars[0]].value < registers[line.vars[1]].value)) *PC = LABELS[line.label]-1;
                val.skip = true;
                break;
            case BR: 
                val.skip = true;
                *PC = LABELS[line.label]-1;
                break;
            case CMP: 
                if(registers[line.vars[1]].value < registers[line.vars[2]].value) val.value = -1;
                else if (registers[line.vars[1]].value == registers[line.vars[2]].value) val.value = 0;
                else if (registers[line.vars[1]].value > registers[line.vars[2]].value) val.value = 1;
                break;
            case HALT:
                *FINISHED = 1;
                val.skip = true;
                break;
            default:
                ;
        }
        *PC = *PC +1;
        *instructionsExecuted = *instructionsExecuted + 1;
        return val;
    }


    bool memoryAccess(executeReturn executedInstruction, Register registers[]){
        return true; // still in order so doesnt matter yet
    }


    void writeBack(executeReturn executedInstruction, Register *registers){
        registers[executedInstruction.storeReg].value = executedInstruction.value;
    }


    /////////////////////////////      Main loop
    void run(){

        // set up 
        Register registers[32];
        float memory[516];
        fill_n(memory, 516, 0);
        string instructionMemory[64];
        int FINISHED = 0;
        int CLOCK = 0;
        int instructionsExecuted = 0;
        int PC = 1;
        string program = "machineCode.txt";
            
        
        // loads the program into the instructionMemory and finds all the 
        // labels for branches, putting them in table: "LABELS"
        unordered_map<string, int> LABELS = loadIntoMemory(instructionMemory, program);
        
        
        //unpipelined loop
        while(FINISHED != 1){

            fetchReturn fetchedInstruction = fetch(instructionMemory, &PC);//fetch
            opline instruction = decode(fetchedInstruction);//decode
            executeReturn executedInstruction = execute(instruction, registers, memory, LABELS, &PC, &FINISHED, &instructionsExecuted);  //execute 
            if(!executedInstruction.skip){
                bool ready = memoryAccess(executedInstruction, registers);//mem access
                if(ready){
                    writeBack(executedInstruction, registers);//writeback
                }
                CLOCK = CLOCK + 5;
            }else{
                CLOCK = CLOCK + 3;
            }
            //print out
            cout << "registers: ";
            for(int i = 0; i<32; i++){
                cout << registers[i].value << " ";
            } 
            cout << endl << "PC: " << PC << endl << endl;
        }
        cout << " clock cycles: " << CLOCK << endl << " instructions executed: " << instructionsExecuted <<  endl << " Program counter: " << PC << endl << " instructions per cycle: " << ((round(float(instructionsExecuted)/float(CLOCK)*100))/100) << endl <<endl;
    } 
};