#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

////reads in a text file and a vector of strings where each line of the text file is a different string
vector<string> readtxtFile(string);

////reads in a string (a line from a text file) and seperates it into words that are seperated by a space
vector<string> split (string);