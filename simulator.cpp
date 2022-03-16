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

float registers[32];
unordered_map<string, int> branches;

//////////////////////////////      Stages

///// fetch simulator
string fetch(vector<string> instructions, int *PC, int *CLOCK){
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
            registers[line.vars[0]] =  registers[line.vars[1]] + registers[line.vars[0]];
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
            *PC = line.vars[0]-1;
            break;
        case CMP: 
            if(registers[line.vars[1]]< registers[line.vars[2]]) registers[line.vars[0]] = -1;
            else if (registers[line.vars[1]] == registers[line.vars[2]]) registers[line.vars[0]] = 0;
            else if (registers[line.vars[1]] > registers[line.vars[2]]) registers[line.vars[0]] = 1;
            break;
        case LABEL:
            // branches[line.vars[0]] = *PC;
            cout << line.label << endl;
            break;
        case BLANK:
            break;
        default:
            ;
    }
    *PC = *PC +1;
    *CLOCK = *CLOCK + 1;
}


/////////////////////////////      Main loop
int main(){

    int CLOCK = 0;
    int instructionsExecuted = 0;
    int PC = 1;
    // loads the program as a vector of strings (one for each line) into "instructions"
    vector<string> instructions = readtxtFile("machineCode.txt");
    while(PC<(instructions.size()+1)){
        //gets the line from instructions corresponding to where the program counter is currently pointing
        string instructionString = fetch(instructions, &PC, &CLOCK);
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
} 