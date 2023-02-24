#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <algorithm>
#include <string>
#include <list>

using namespace std;

int main(int argc, char** argv) {

    if (argc != 3) {
        cout << "Usage: " << argv[0] << "<k> <file>" << endl;
        return 1;
    }

    int k = (int) argv[1];
    string fileName = argv[2];

    read_file(k, fileName);

    return 0;
}


void read_file(int k, string fileName) {

    ifstream file(fileName);

    char c;

    map<string, list<int>> map; // map dos padroes, lista das posições onde forem encontradas

    while (!file.eof())
    {

        c = file.get();//le o caracter


    }
    
    file.close();
}

double calculatePrediction(){

    return 0;
}