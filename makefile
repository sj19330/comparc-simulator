exec: simulator.o convert.o parser.o nonPL.o pipelined.o
	g++ simulator.o convert.o parser.o nonPL.o pipelined.o -o exec

pipelined.o: pipelined.cpp
	g++ -c pipelined.cpp

nonPL.o: nonPL.cpp
	g++ -c nonPL.cpp

simulator.o: simulator.cpp
	g++ -c simulator.cpp

convert.o: convert.cpp headers/convert.h
	g++ -c convert.cpp

parser.o: parser.cpp headers/parser.h
	g++ -c parser.cpp

clean:
	rm *.o output


# taret: dependencies
# 	action