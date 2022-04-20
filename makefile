exec: simulator.o convert.o parser.o superscalar.o pipelined.o nonPL.o
	g++ simulator.o convert.o parser.o superscalar.o pipelined.o nonPL.o -o exec

nonPL.o: nonPL.cpp
	g++ -c nonPL.cpp

pipelined.o: pipelined.cpp nonPL.cpp
	g++ -c pipelined.cpp 

superscalar.o: superscalar.cpp pipelined.cpp nonPL.cpp
	g++ -c superscalar.cpp

simulator.o: simulator.cpp superscalar.cpp pipelined.cpp nonPL.cpp
	g++ -c simulator.cpp

convert.o: convert.cpp headers/convert.h
	g++ -c convert.cpp

parser.o: parser.cpp headers/parser.h
	g++ -c parser.cpp

clean:
	rm *.o output


# taret: dependencies
# 	action