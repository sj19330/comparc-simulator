#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include "headers/convert.h"

using namespace std;


op decodeOp(string op){
    if(op == "Add") return ADD;
    else if(op == "Addi")return ADDI;
    else if(op == "Sub")return SUB;
    else if(op == "Subi")return SUBI;
    else if(op == "Mul")return MUL;
    else if(op == "Muli")return MULI;
    else if(op == "Div")return DIV;
    else if(op == "Divi")return DIVI;
    else if(op == "Mod")return MOD;
    else if(op == "Ld")return LD;
    else if(op == "Ldi")return LDI;
    else if(op == "Str")return STR;
    else if(op == "Brlt")return BRLT;
    else if(op == "Brne")return BRNE;
    else if(op == "Bre")return BRE;    
    else if(op == "Br")return BR;
    else if(op == "Cmp")return CMP;
    else if(op == "Label:") return LABEL;
    else return BLANK;
}

int decodeReg(string r){
    string x;
    if(r[0] == 'r'){
        for(int i = 1; i<r.size(); i++){
            x.push_back(r[i]);
        }
    }else{x = r;}
    return stoi(x);
}

