#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include "nonPL.cpp"
#include <queue>



using namespace std;

class Pipeline:public NonPipeline{
public:
    
    ///// gets the line from instructions corresponding to where the program counter is currently pointing
    fetchReturn fetch(string instructions[], int *PC){
        fetchReturn val;
        val.instruction = instructions[(*PC-1)];
        val.branch = checkIfBranch(val.instruction);
        *PC = *PC + 1;
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

    bool execute(opline line, float *registers, float *memory, unordered_map<string, int> LABELS, int *PC, int *FINISHED){
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
                if(registers[line.vars[0]] != registers[line.vars[1]]) *PC = LABELS[line.label];
                break;
            case BRE:
                if(registers[line.vars[0]] == registers[line.vars[1]]) *PC = LABELS[line.label];
                break;
            case BRLT:
                if(!(registers[line.vars[0]] < registers[line.vars[1]])) *PC = LABELS[line.label];
                break;
            case BR: 
                *PC = LABELS[line.label];
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
        return line.branch;
    }


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
        string program = "machineCode.txt";
        int programLength;

        int PC = 1;
        bool branch = false;
            
        
        // loads the program into the instructionMemory and finds all the 
        // labels for branches, putting them in table: "LABELS"
        // also fills program length with the length of the program
        unordered_map<string, int> LABELS = loadIntoMemory(instructionMemory, program, &programLength);
         cout << LABELS["B1"] << endl;

        queue<fetchReturn> decodeInput;
        queue<opline> executeInput;
        
        //unpipelined loop
        while(FINISHED != 1){

            int thisPC = PC;
            cout << endl << "PC: " << PC << endl;
            bool decodeHasBeenRun = false;
            bool fetchHasBeenRun = false;
            fetchReturn fetched;
            opline instruction;
            opline executable;
            fetchReturn nextDecode;

            if(!branch){
                //fetch
                if(thisPC <= programLength){
                    fetched = fetch(instructionMemory, &thisPC);
                    fetchHasBeenRun = true;
                    branch = fetched.branch;
                }
            }
            //decode
            if(decodeInput.size() > 0){
                nextDecode = decodeInput.front();
                decodeInput.pop();
                instruction = decode(nextDecode);
                decodeHasBeenRun = true;
            }
            
            //execute
            if(executeInput.size() > 0){
                executable = executeInput.front();
                executeInput.pop();
                bool b = execute(executable, registers, memory, LABELS, &thisPC, &FINISHED); 
                if(b){
                    branch = false;
                }
                instructionsExecuted++;
            }

            //update
            if(fetchHasBeenRun){
                decodeInput.push(fetched);
            }
            if(decodeHasBeenRun){
                executeInput.push(instruction);
            }
            PC = thisPC;

            //print out
            CLOCK = CLOCK + 1;
            cout << "Fetched instruction: " << fetched.instruction << endl;
            cout << "Decoded instruction: " << nextDecode.instruction << endl;
            cout << "Executed instruction: " << executable.operation << endl;
            cout << "registers: ";
            for(int i = 0; i<32; i++){
                cout << registers[i] << " ";
            } 
            cout << endl << "Clock cycles: " << CLOCK << endl;
            cout << "PC: " << PC << endl << endl;
            
        }

        cout << " clock cycles: " << CLOCK << endl << " instructions executed: " << instructionsExecuted <<  endl << " Program counter: " << PC << endl << " instructions per cycle: " << ((round(float(instructionsExecuted)/float(CLOCK)*100))/100) << endl << " Pipelined" << endl <<endl;
    } 
};
