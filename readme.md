# Computer Processor Simulator

 This project simulates the processor of a modern pipelined computer
 and runs assembly code stored in the machineCode.txt file
 
 The pipeline has 4 stages: 
 - Fetch
 - Decode
 - Execute
 - Writeback

This project was a final year project for the 'Advanced Computer Architecture' moudule whilst studyng at the university of bristol which achieved a high 2.1



##### Instructions on how to use: 

> to compile run: "make", then to run executable run: "./exec"
> the simulator.cpp file is the main file for the program
> 
> 
> the "machineCode.txt" file is where the program that is executed by the code is held
> 
> you can copy and paste any of the programs in the "programs.txt" file to run different programs
> 
> the "parser.cpp"  & "parser.h" files contain two helperfunctions for the simulator. They help to read in the machineCode and seperate each line for the simulator to understand
> 
> the "convert.cpp" & "convert.h" files contain the enums for each executable operation of the instruction set architecture. they also contain functions for converting different parts of a string into what is needed for the 
> simulator. 
> 
> the GCD, factorial & fibonacci programs are contained in the programs folder
> to run, copy the code into "machineCode.txt"
> 
> the code already in the "machineCode.txt" file is the code for GCD and can be run straight away
> 
> the simulator has 4 stages. The first, fetch gets the instruction that the program counter is pointing to and returns it as a string. 
> the seccond, decode, splits this string into words and, depending on what the operation is, returns an opline struct
> which contains the operation, the registers needed to be accessed and any ints which may be used for imediate instructions. 
> the execute stage then performs actions based on the opline struct passed in. The fourth, writeback then puts the calculated value into the 
> correct register or space in memory
> 
> you can change the simulator from superscalar back to non pipelined or pipelined by changing the setting variable 
> in simulator.cpp to "nonPL" or "pipelined" respectivly
