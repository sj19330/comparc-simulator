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

    struct opline {
        op operation;
        vector<int> vars;
        string label;
        bool branch;
    } typedef opline;

    struct fetchReturn{
        string instruction;
        bool branch;
    } typedef fetchReturn;

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
        cout << fetchedInstruction.instruction  << endl;
        return opline; 
    }

    ///// execute simulator 
    ///// takes the opcode and vars and executes accordingly
    void execute(opline line, float *registers, float *memory, unordered_map<string, int> LABELS, int *PC, int *FINISHED){
        float val;
        switch(line.operation){
            case ADD:
                registers[line.vars[0]] =  registers[line.vars[1]] + registers[line.vars[2]];
                break;
            case ADDI:
                registers[line.vars[0]] =  registers[line.vars[1]] + line.vars[2];
                break;
            case SUB:
                registers[line.vars[0]] =  registers[line.vars[1]] - registers[line.vars[2]];
                break;
            case SUBI:
                registers[line.vars[0]] =  registers[line.vars[1]] - line.vars[2];
                break;
            case MUL: 
                registers[line.vars[0]] =  registers[line.vars[1]] * registers[line.vars[2]];
                break;
            case MULI: 
                registers[line.vars[0]] =  registers[line.vars[1]] * line.vars[2];
                break;
            case DIV:
                registers[line.vars[0]] =  registers[line.vars[1]] / registers[line.vars[2]];
                break;
            case DIVI:
                registers[line.vars[0]] =  registers[line.vars[1]] / line.vars[2];
                break;
            case MOD:
                registers[line.vars[0]] = int(registers[line.vars[1]]) % int(registers[line.vars[2]]);
                break;
            case LD:
                registers[line.vars[0]] = memory[int(registers[line.vars[1]]) + int(registers[line.vars[2]])];
            case LDI:
                cout << line.vars[0] << endl;
                registers[line.vars[0]] = line.vars[1];
                break;
            case MV:
                registers[line.vars[0]] = registers[line.vars[1]];
                break;
            case STR:
            // add somthing to make sure the values are ints before 
                memory[int(registers[line.vars[1]]) + int(registers[line.vars[2]])] = registers[line.vars[0]];
                break;
            case STRI:
                memory[int(registers[line.vars[1]]) + int(registers[line.vars[2]])] = line.vars[0];
                break;
            case BRNE:
                if(registers[line.vars[0]] != registers[line.vars[1]]) *PC = LABELS[line.label]-1;
                break;
            case BRE:
                if(registers[line.vars[0]] == registers[line.vars[1]]) *PC = LABELS[line.label]-1;
                break;
            case BRLT:
                if(!(registers[line.vars[0]] < registers[line.vars[1]])) *PC = LABELS[line.label]-1;
                break;
            case BR: 
                cout << LABELS[line.label] << endl;
                *PC = LABELS[line.label]-1;
                break;
            case CMP: 
                if(registers[line.vars[1]]< registers[line.vars[2]]) registers[line.vars[0]] = -1;
                else if (registers[line.vars[1]] == registers[line.vars[2]]) registers[line.vars[0]] = 0;
                else if (registers[line.vars[1]] > registers[line.vars[2]]) registers[line.vars[0]] = 1;
                break;
            case HALT:
                *FINISHED = 1;
            case BLANK:
                break;
            default:
                ;
        }
        *PC = *PC +1;
        cout << *PC << endl;
    }


    /////////////////////////////      Main loop
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
            
        
        // loads the program into the instructionMemory and finds all the 
        // labels for branches, putting them in table: "LABELS"
        unordered_map<string, int> LABELS = loadIntoMemory(instructionMemory, program);
        
        
        //unpipelined loop
        while(FINISHED != 1){

            fetchReturn fetchedInstruction = fetch(instructionMemory, &PC);
            opline instruction = decode(fetchedInstruction);
            cout << "Branch: " << instruction.branch << endl;
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
        cout << " clock cycles: " << CLOCK << endl << " instructions executed: " << instructionsExecuted <<  endl << " Program counter: " << PC << endl << " instructions per cycle: " << (float(instructionsExecuted)/float(CLOCK)) << endl <<endl;
    } 
};