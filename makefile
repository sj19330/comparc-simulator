exec: simulator.o convert.o parser.o
	g++ simulator.o convert.o parser.o -o exec

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