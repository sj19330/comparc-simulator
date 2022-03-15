#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include "headers/convert.h"
#include "headers/parser.h"
#include <unordered_map>


using namespace std;

float registers[32];
unordered_map<string, int> branches;

struct opline {
    op operation;
    vector<string> vars;
} typedef opline;

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
    vector<string> elements = split(line);
    opline.operation = decodeOp(elements[0]);
    elements.erase(elements.begin());
    opline.vars = elements;
    *CLOCK = *CLOCK + 1;
    cout << line  << endl;
    return opline;
}

///// execute simulator 
void execute(opline line, float registers[32], int *CLOCK, int *PC){
    float val;
    switch(line.operation){
        case ADD:
            registers[decodeReg(line.vars[0])] =  registers[decodeReg(line.vars[1])] + registers[decodeReg(line.vars[0])];
            break;
        case ADDI:
            registers[decodeReg(line.vars[0])] =  registers[decodeReg(line.vars[1])] + decodeReg(line.vars[2]);
            break;
        case SUB:
            registers[decodeReg(line.vars[0])] =  registers[decodeReg(line.vars[1])] - registers[decodeReg(line.vars[2])];
            break;
        case SUBI:
            registers[decodeReg(line.vars[0])] =  registers[decodeReg(line.vars[1])] - decodeReg(line.vars[2]);
            break;
        case MUL: 
            registers[decodeReg(line.vars[0])] =  registers[decodeReg(line.vars[1])] * registers[decodeReg(line.vars[2])];
            break;
        case MULI: 
            registers[decodeReg(line.vars[0])] =  registers[decodeReg(line.vars[1])] * decodeReg(line.vars[2]);
            break;
        case DIV:
            registers[decodeReg(line.vars[0])] =  registers[decodeReg(line.vars[1])] / registers[decodeReg(line.vars[2])];
            break;
        case DIVI:
            registers[decodeReg(line.vars[0])] =  registers[decodeReg(line.vars[1])] / decodeReg(line.vars[2]);
            break;
        case MOD:
            registers[decodeReg(line.vars[0])] = int(registers[decodeReg(line.vars[1])]) % int(registers[decodeReg(line.vars[2])]);
            break;
        case LD:
            registers[decodeReg(line.vars[0])] = registers[decodeReg(line.vars[1])];
            break;
        case LDI:
            cout << line.vars[0] << endl;
            registers[decodeReg(line.vars[0])] = decodeReg(line.vars[1]);
            break;
        case BRNE:
            if(registers[decodeReg(line.vars[1])] != registers[decodeReg(line.vars[2])]) *PC = decodeReg(line.vars[0])-1;
            break;
        case BRE:
            if(registers[decodeReg(line.vars[1])] == registers[decodeReg(line.vars[2])]) *PC = decodeReg(line.vars[0])-1;
            break;
        case BRLT:
            if(!(registers[decodeReg(line.vars[1])] < registers[decodeReg(line.vars[2])])) *PC = decodeReg(line.vars[0])-1;
            break;
        case BR: 
            *PC = decodeReg(line.vars[0])-1;
            break;
        case CMP: 
            if(registers[decodeReg(line.vars[1])]< registers[decodeReg(line.vars[2])]) registers[decodeReg(line.vars[0])] = -1;
            else if (registers[decodeReg(line.vars[1])] == registers[decodeReg(line.vars[2])]) registers[decodeReg(line.vars[0])] = 0;
            else if (registers[decodeReg(line.vars[1])] > registers[decodeReg(line.vars[2])]) registers[decodeReg(line.vars[0])] = 1;
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
    vector<string> instructions = readtxtFile("machineCode.txt");
    while(PC<(instructions.size()+1)){
        string instructionString = fetch(instructions, &PC, &CLOCK);
        opline instruction = decode(instructionString, &CLOCK);
        execute(instruction, registers, &CLOCK, &PC);   
        instructionsExecuted++;
        for(int i = 0; i<32; i++){
            cout << registers[i] << " ";
        }
        cout << endl << PC << endl << endl;
    }
    cout << "clock cycles: " << CLOCK << endl << " instructions executed: " << instructionsExecuted <<  endl << " Program counter: " << PC << endl << " instructions per cycle: " << (float(instructionsExecuted)/float(CLOCK)) << endl <<endl;
}