#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;
//// op defines the ISA
enum op {
    ADD, ADDI, SUB, SUBI, MUL, MULI, DIV, DIVI, MOD, LD, LDI, STR, BRLT, BRNE, BRE, BR, CMP, BLANK, LABEL, HALT
};

// decodeOp converts a string into an enum of type op so it can later be used in a switch statement
op decodeOp(string op);

// decodeReg converts the string of a register so only the register number is left
int decodeReg(string r);
