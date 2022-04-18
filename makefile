exec: simulator.o convert.o parser.o pipelined.o nonPL.o
	g++ simulator.o convert.o parser.o pipelined.o nonPL.o -o exec

nonPL.o: nonPL.cpp
	g++ -c nonPL.cpp

pipelined.o: pipelined.cpp
	g++ -c pipelined.cpp 

simulator.o: simulator.cpp pipelined.cpp
	g++ -c simulator.cpp

convert.o: convert.cpp headers/convert.h
	g++ -c convert.cpp

parser.o: parser.cpp headers/parser.h
	g++ -c parser.cpp

clean:
	rm *.o output


# taret: dependencies
# 	action