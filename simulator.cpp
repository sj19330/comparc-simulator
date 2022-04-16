#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include "headers/convert.h"
#include "headers/parser.h"
#include <unordered_map>


using namespace std;

struct opline {
    op operation;
    vector<int> vars;
    string label;
} typedef opline;

int LENOFPROGRAM;
int FINISHED = 0;
float registers[32];
string instructionMemory[64];
float memory[516];
unordered_map<string, int> LABELS;
//////////////////////////////      Loading in program and initialising


void loadIntoMemory(string program){
    vector<string> instructions = readtxtFile(program);
    LENOFPROGRAM = instructions.size();
    for(int i = 0; i<instructions.size(); i++){
        instructionMemory[i] = instructions[i];
    }
}


void findLabels(string insts[64]){
    cout << "here" << endl;
    for(int i=0; i<LENOFPROGRAM; i++){
        vector<string> line = split(insts[i]);
        if(line[0] == "Label:"){
            LABELS[line[1]] = i+2;
        }
    }
}
//////////////////////////////      Stages

///// fetch simulator
string fetch(string instructions[], int *PC, int *CLOCK){
    string instruction = instructions[(*PC-1)];
    split(instruction);
    *CLOCK = *CLOCK + 1;
    return instruction;
}

///// decode simulator
opline decode(string line, int *CLOCK){
    opline opline;
    vector<int> decodedElem;
    vector<string> elements = split(line);

    opline.operation = decodeOp(elements[0]);
    if(opline.operation == BR){
        opline.label = elements[1];
        elements.erase(elements.begin());
    }
    elements.erase(elements.begin());
    if(opline.operation != LABEL){
        for(int i = 0; i < elements.size(); i++){
            int elem = decodeReg(elements[i]);
            decodedElem.push_back(elem);
        }
    }else{
        opline.label = elements[0];
    }
    opline.vars = decodedElem;
    *CLOCK = *CLOCK + 1;
    cout << line  << endl;
    return opline;
}

///// execute simulator 
void execute(opline line, float registers[32], int *CLOCK, int *PC){
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
            registers[line.vars[0]] = registers[line.vars[1]];
            break;
        case LDI:
            cout << line.vars[0] << endl;
            registers[line.vars[0]] = line.vars[1];
            break;
        case BRNE:
            if(registers[line.vars[1]] != registers[line.vars[2]]) *PC = line.vars[0]-1;
            break;
        case BRE:
            if(registers[line.vars[1]] == registers[line.vars[2]]) *PC = line.vars[0]-1;
            break;
        case BRLT:
            if(!(registers[line.vars[1]] < registers[line.vars[2]])) *PC = line.vars[0]-1;
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
        case LABEL:
            break;
        case HALT:
            FINISHED = 1;
        case BLANK:
            break;
        default:
            ;
    }
    *PC = *PC +1;
    cout << *PC << endl;
    *CLOCK = *CLOCK + 1;
}


/////////////////////////////      Main loop
int main(){


    int CLOCK = 0;
    int instructionsExecuted = 0;
    int PC = 1;
    string program = "machineCode.txt";
    
    
    // loads the program into the instructionMemory and finds all the labels for branches, putting them in table: "LABELS"
    loadIntoMemory(program);
    
    // finds where the Labels are so they can easily be accesed for branches
    findLabels(instructionMemory);


    while(FINISHED != 1){
        //gets the line from instructions corresponding to where the program counter is currently pointing
        string instructionString = fetch(instructionMemory, &PC, &CLOCK);
        //splits the line into an opcode and variables for the computer to execute
        opline instruction = decode(instructionString, &CLOCK);
        //takes the opcode and vars and executes accordingly
        execute(instruction, registers, &CLOCK, &PC);   
        instructionsExecuted++;
        for(int i = 0; i<32; i++){
            cout << registers[i] << " ";
        } 
        cout << endl << PC << endl << endl;
    }
    cout << " clock cycles: " << CLOCK << endl << " instructions executed: " << instructionsExecuted <<  endl << " Program counter: " << PC << endl << " instructions per cycle: " << (float(instructionsExecuted)/float(CLOCK)) << endl <<endl;
    cout << LABELS["B1"] <<endl;
} 

// int main(){
//     string program = "machineCode.txt";
//     loadIntoMemory(program);
//     findLabels(instructionMemory);
// }