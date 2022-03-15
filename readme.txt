to compile the "simulator.cpp" file run: "make", then to run executable run: "./exec"
this is the main file for the program

the simulator prints out the line from the "machineCode.txt" file, the register values, and the program counter
for each instruction.

the "machineCode.txt" file is where the program that is executed by the code is held

you can copy and paste any of the programs in the "programs.txt" file to run different programs

the "parser.cpp"  & "parser.h" files contain two helperfunctions for the simulator. They help to read in the machineCode and seperate each line for the simulator to understand

the "convert.cpp" & "convert.h" files contain the enums for each executable operation. they also contain functions for converting different parts of a string into what is needed for the 
simulator. 

the GCD, factorial & accumulator_loop all have comments to help explain the program, these will have to be removed 
before copying the code into "machineCode.txt" for the code to run 

the code already in the "machineCode.txt" file is the code for GCD and can be run straight away

the simulator has 3 stages. The first, fetch gets the instruction that the program counter is pointing to and returns it as a string. 
the seccond, decode, splits this string into words and, depending on what the operation is, returns an opline struct
which contains the operation, the registers needed to be accessed and any ints which may be used for imediate instructions. 
the execute stage then performs actions based on the opline struct passed in. 
