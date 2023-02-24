#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <algorithm>
#include <string>
#include <list>

using namespace std;

void read_file(int k, string fileName);
double calculatePrediction();
void printMap(map<string, list<int>> map);

int main(int argc, char** argv) {

    if (argc != 3) {
        cout << "Usage: " << argv[0] << "<k> <file>" << endl;
        return 1;
    }

    int k = stoi(argv[1]);
    string fileName = argv[2];

    read_file(k, fileName);

    return 0;
}


void read_file(int k, string fileName) {

    ifstream file(fileName);

    char c;

    map<string, list<int>> map; // map dos padroes, lista das posições onde forem encontradas

    string pattern = "";

    int filePos = 1;
    while (!file.eof())
    {

        c = file.get();//le o caracter

        pattern += c; // adiciona o caracter ao padrao

        if (pattern.size() >= k) {

            if (map.count(pattern) == 0) { //padrao nao esta no map

                list<int> tmp; //! maybe usar arraylist, fica + facil
                tmp.push_back(filePos);
                map.insert({pattern, tmp}); // insere o padrao no map com a posição atual

            } else { //padrao ja esta no map

                map[pattern].push_back(filePos); // adiciona a posição atual ao padrao

            }

            pattern.erase(0, 1); // remove o primeiro caracter do padrao

        }


        filePos++;
    }
    
    file.close();

    printMap(map);
}

double calculatePrediction(){

    return 0;
}

void printMap(map<string, list<int>> map) {

    for (auto it = map.begin(); it != map.end(); it++) {
        cout << it->first << " : ";
        for (auto it2 = it->second.begin(); it2 != it->second.end(); it2++) {
            cout << *it2 << " ";
        }
        cout << endl;
    }
}