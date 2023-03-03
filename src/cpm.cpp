#include <iostream>
#include <fstream>
#include <map>
#include <algorithm>
#include <string>
#include <list>
#include <vector>

using namespace std;

void first_read(string fileName);
void cpm(int k);
double calculatePrediction();
void printMap(map<string, list<int>> map);
void printAlphabet(map<char, int> map);

vector<int> arr_file;
map<char, int> alphabet;

int main(int argc, char** argv) {

    if (argc != 3) {
        cout << "Usage: " << argv[0] << "<k> <file>" << endl;
        return 1;
    }

    int k = stoi(argv[1]);
    string fileName = argv[2];

    first_read(fileName);

    cpm(k);

    return 0;
}

void first_read(string fileName) {

    ifstream file(fileName);

    char c = file.get();
    

    while (!file.eof()) {

        arr_file.push_back(c);

        alphabet.insert({c, 0});
        alphabet[c]++;

        c = file.get();
    }

    file.close();
}


void cpm(int k) {

    char c;

    map<string, list<int>> pointer_map; // map dos padroes, lista das posições onde forem encontradas

    string pattern = "";

    for (int pos = 0; pos < arr_file.size(); pos++)
    {

        c = arr_file[pos];//le o caracter

        pattern += c; // adiciona o caracter ao padrao

        if (pattern.size() >= k) {

            if (pointer_map.count(pattern) == 0) { //padrao nao esta no map

                list<int> tmp; //! maybe usar arraylist, fica + facil
                tmp.push_back(pos);
                pointer_map.insert({pattern , tmp}); // insere o padrao no map com a posição atual

            } else { //padrao ja esta no map

                pointer_map[pattern].push_back(pos); // adiciona a posição atual ao padrao

            }

            pattern.erase(0, 1); // remove o primeiro caracter do padrao

        }

    }

    //printMap(map);
    printAlphabet(alphabet);
}

double calculatePrediction() {

    return 0;
}

void printAlphabet(map<char, int> pointer_map) {

    for (auto it = pointer_map.begin(); it != pointer_map.end(); it++) {
        cout << it->first << " : " << it->second << endl;
    }
}

void printMap(map<string, list<int>> pointer_map) {

    for (auto it = pointer_map.begin(); it != pointer_map.end(); it++) {
        cout << it->first << " : ";
        for (auto it2 = it->second.begin(); it2 != it->second.end(); it2++) {
            cout << *it2 << " ";
        }
        cout << endl;
    }
}