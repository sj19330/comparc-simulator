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
    int s1;
    int s2;
    int store;
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
    
    if((opline.operation == BRNE)||(opline.operation == BRLT)||(opline.operation == BR)||(opline.operation == BRE)){
        opline.store = stoi(elements[1]);
    }else{
        opline.store = decodeReg(elements[1]);
    }
    
    if((opline.operation == LDI)||(opline.operation == BR)){
        opline.s1 = stoi(elements[2]);
    }
    else if(opline.operation == LD){
        opline.s1 = decodeReg(elements[2]);
    }
    else{
        opline.s1 = decodeReg(elements[2]);
        if((opline.operation == ADDI)||(opline.operation == SUBI)||(opline.operation == MULI)||(opline.operation == DIVI)){
            opline.s2 = stoi(elements[3]);
        }else{
            opline.s2 = decodeReg(elements[3]); 
        }
    }
    
    
    *CLOCK = *CLOCK + 1;
    cout << line << "      "  << endl;
    // cout << opline.operation << " " << opline.store << " " << opline.s1 << " " << opline.s2 << endl;    
    return opline;
}

///// execute simulator 
void execute(opline o, float registers[32], int *CLOCK, int *PC){
    float val;
    switch(o.operation){
        case ADD:
            registers[o.store] =  registers[o.s1] + registers[o.s2];
            break;
        case ADDI:
            registers[o.store] =  registers[o.s1] + o.s2;
            break;
        case SUB:
            registers[o.store] =  registers[o.s1] - registers[o.s2];
            break;
        case SUBI:
            registers[o.store] =  registers[o.s1] - o.s2;
            break;
        case MUL: 
            registers[o.store] =  registers[o.s1] * registers[o.s2];
            break;
        case MULI: 
            registers[o.store] =  registers[o.s1] * o.s2;
            break;
        case DIV:
            registers[o.store] =  registers[o.s1] / registers[o.s2];
            break;
        case DIVI:
            registers[o.store] =  registers[o.s1] / o.s2;
            break;
        case MOD:
            registers[o.store] = int(registers[o.s1]) % int(registers[o.s2]);
            break;
        case LD:
            registers[o.store] = registers[o.s1];
            break;
        case LDI:
            registers[o.store] = o.s1;
            break;
        case BRNE:
            if(registers[o.s1] != registers[o.s2]) *PC = o.store-1;
            break;
        case BRE:
            if(registers[o.s1] == registers[o.s2]) *PC = o.store-1;
            break;
        case BRLT:
            if(!(registers[o.s1] < registers[o.s2])) *PC = o.store-1;
            break;
        case BR: 
            *PC = o.store-1;
            break;
        case CMP: 
            if(registers[o.s1]< registers[o.s2]) registers[o.store] = -1;
            else if (registers[o.s1] == registers[o.s2]) registers[o.store] = 0;
            else if (registers[o.s1] > registers[o.s2]) registers[o.store] = 1;
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
    vector<string> instructions = readtxtFile();
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
    cout << "clock cycles: " << CLOCK << " instructions executed: " << instructionsExecuted << " Program counter: " << PC << endl <<endl;
}