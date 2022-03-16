#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include "convert.h"

using namespace std;

struct opline {
    op operation;
    vector<int> vars;
    string label;
} typedef opline;

////reads in a text file and a vector of strings where each line of the text file is a different string
vector<string> readtxtFile(string);

////reads in a string (a line from a text file) and seperates it into words that are seperated by a space
vector<string> split (string);