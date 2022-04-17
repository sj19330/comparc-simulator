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
    } typedef opline;

    //////////////////////////////      Loading in program and initialising
    unordered_map<string, int> loadIntoMemory(string *instructionMemory, string program){
        vector<string> instructions = readtxtFile(program);
        unordered_map<string, int> LABELS;
        int j = 0;
        for(int i = 0; i<instructions.size(); i++){
            vector<string> line = split(instructions[i]);
            if(line[0] == "Label:"){
                LABELS[line[1]] = j+1;
            }
            else{
                instructionMemory[j] = instructions[i];
                j++;
            }   
        }
        return LABELS;
    }

    //////////////////////////////      Stages

    ///// fetch simulator
    string fetch(string instructions[], int *PC){
        string instruction = instructions[(*PC-1)];
        split(instruction);
        return instruction;
    }

    ///// decode simulator
    opline decode(string line){
        opline opline;
        vector<int> decodedElem;
        vector<string> elements = split(line);

        opline.operation = decodeOp(elements[0]);
        if(opline.operation == BR || opline.operation == BRE || opline.operation == BRNE || opline.operation == BRLT){
            opline.label = elements[1];
            elements.erase(elements.begin());
        }
        elements.erase(elements.begin());
        for(int i = 0; i < elements.size(); i++){
            int elem = decodeReg(elements[i]);
            decodedElem.push_back(elem);
        }
        opline.vars = decodedElem;
        cout << line  << endl;
        return opline; 
    }

    ///// execute simulator 
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
        
        
        while(FINISHED != 1){
            //gets the line from instructions corresponding to where the program counter is currently pointing
            string instructionString = fetch(instructionMemory, &PC);
            //splits the line into an opcode and variables for the computer to execute
            opline instruction = decode(instructionString);
            //takes the opcode and vars and executes accordingly
            execute(instruction, registers, memory, LABELS, &PC, &FINISHED);   
            instructionsExecuted++;
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