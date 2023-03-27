#include <iostream>
#include <fstream>
#include <map>
#include <algorithm>
#include <string>
#include <list>
#include <vector>
#include <cmath>
#include<time.h>
#include "unistd.h"
#include "cpm_gen.hpp"

using namespace std;

map<char, int> alphabet;
int count_alphabet = 0;
map<string, vector<char> > map_patterns;

bool OPT_train=false;
bool OPT_lower=false;

int num_guess = 0;

int main(int argc, char** argv) {
    srand(time(0)); //! seed for random number generator -> aleatorio

    int c;
    

    string ini_str="";
    string train_file="";
    int num_char = 0;

    
    while ((c = getopt(argc, argv, "htls:f:n:")) != -1){
        switch(c){
            case 'h':
                printUsage(argv[0]);
                return 0;
            case 's': 
                ini_str=optarg;
                break;
            case 'f': 
                train_file=optarg;
                break;
            case 'n':
                num_char = stoi(optarg);
                break;
            case 't':
                OPT_train=true;
                break;
            case 'l':
                OPT_lower=true;
                break;
                            
            case '?':
                printUsage(argv[0]);
                return 1;
        }
    }

    if (ini_str == "" || train_file == "" || num_char <= 0) {
        printUsage(argv[0]);
        return 1;
    }

    cout << "String inicial: " << ini_str << endl;
    cout << "Train file: " << train_file << endl;
    cout << "Num caracteres a gerar: " << num_char << endl;
    cout << "Alow training himself: " << OPT_train << endl;
    cout << "Alow only lower case letters: " << OPT_lower << endl;
    cout << endl;
    cout << "Working..." << endl;
    cout << endl;
    

    firstPass(train_file,ini_str.size());


    //printMap(map_patterns); //! DEBUG
    //cout << endl;
    //printAlphabet(alphabet);  //! DEBUG

    cpm_gen(ini_str,ini_str.size(), num_char);

    cout << "Done!" << endl;
    cout << "Num guesses: " << num_guess << "/" << num_char << endl;
    cout << "Output file: out.txt" << endl;

    return 0;
}

void firstPass(string fileName,int k) {

    ifstream file(fileName);

    string pattern = "";
    char c = file.get();

    if (OPT_lower) {
        c = tolower(c);
    }

    
    while (!file.eof()) {
        count_alphabet++;           // for calculating the base distribution
        alphabet.insert({c, 0});    // for calculating the base distribution
        alphabet[c]++;              // for calculating the base distribution

        pattern += c; 

        if(pattern.size() >= k+1) { // k+1 para incluir o caracter seguinte ao padrao

            map_patterns[pattern.substr(0,k)].push_back(pattern[k]);

            pattern.erase(0, 1); // remove o primeiro caracter do padrao

        }


        c = file.get();

        if (OPT_lower) {
            c = tolower(c);
        }
        
    }

    file.close();
}


void cpm_gen(string ini_str,int k,int num_char) {

    ofstream OutFile("out.txt");
    
    OutFile << ini_str << "|";

    int char_counter = 0;
    string pattern = ini_str;

    while (char_counter<num_char) {

        char put_char;
        
        if (map_patterns.count(pattern) == 0) { //padrao nao esta no map

            num_guess++;

            // mandar um caracter "aleatorio"
            double max = 0;
            int total_simbols = 0;
            for(std::map<char,int>::iterator it = alphabet.begin(); it != alphabet.end(); ++it) {
                total_simbols += it->second;
            }

            int random = rand() % total_simbols; // gera um numero aleatorio entre 0 e total_simbols-1

            int sum = 0;
            for(std::map<char,int>::iterator it = alphabet.begin(); it != alphabet.end(); ++it) {
                if (sum <= random && random < sum + alphabet[it->first]) {
                    put_char = it->first;
                    break;
                }
                sum += alphabet[it->first];
            }

            if (OPT_train){
                // aprender com ele mesmo -> por o padrao no map com simbolo aleatorio como proximo
                map_patterns[pattern].push_back(put_char);     
            }

        } else { //padrao ja esta no map

            map <char, int> tmp;

            int total_simbols = 0;
            for (char c : map_patterns[pattern]) {

                if (c == '\0') {
                    continue;
                }

                tmp[c]++;
                total_simbols++;
            }

            if (total_simbols == 0) { //! DEBUG
                cout << "ERRO: total_simbols = 0" << endl;
            }

            int random = rand() % total_simbols; // gera um numero aleatorio entre 0 e total_simbols-1

            int sum = 0;
            for(std::map<char,int>::iterator it = alphabet.begin(); it != alphabet.end(); ++it) {
                if (sum <= random && random < sum + tmp[it->first]) {
                    put_char = it->first;
                    break;
                }
                sum += tmp[it->first];
            }


            if (OPT_train){
                // aprender com o caracter gerado -> por o padrao no map com simbolo gerado como proximo
                map_patterns[pattern].push_back(put_char);
            }
            
        }

        OutFile << put_char;

        if (put_char == '\0') //! DEBUG
        {
            cout << "ERRO: put_char = \\0" << endl;
        }
        
        char_counter++;
        pattern.erase(0, 1); // remove o primeiro caracter do padrao
        pattern += put_char;

        count_alphabet++;                   // for calculating the base distribution
        alphabet.insert({put_char, 0});     // for calculating the base distribution
        alphabet[put_char]++;               // for calculating the base distribution
    }


    OutFile.close();
}


void printUsage(string file) {
    cout << "Usage: " << file << " -s 'string inicial' -f train_file.txt -n num_caracteres_a_gerar [OPTIONS]" << endl;
    cout << "Example: " << file << " -s 'As the' -f othello.txt -n 2000" << endl;
    cout << "Options:" << endl;
    cout << "  -h  help" << endl;
    cout << "  -t  (alow training himself)" << endl;
    cout << "  -l  (alow only lower case letters - better for smaller input texts)" << endl;
}


void printAlphabet(map<char, int> pointer_map) {
    cout << "Alphabet: " << endl;

    for (auto it = pointer_map.begin(); it != pointer_map.end(); it++) {
        cout << it->first << " : " << it->second << endl;
    }

}


void printMap(map<string, vector<char>> pointer_map) {

    for (auto it = pointer_map.begin(); it != pointer_map.end(); it++) {
        cout << it->first << " : ";
        for (char c : it->second) {
            cout << "|" << c << "|";
        }
        cout << endl;
    }
}