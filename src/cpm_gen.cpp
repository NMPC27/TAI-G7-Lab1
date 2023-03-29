#include <iostream>
#include <fstream>
#include <map>
#include <algorithm>
#include <string>
#include <list>
#include <vector>
#include <cmath>
#include <time.h>
#include <unistd.h>
#include "cpm_gen.hpp"

using namespace std;

// number of occurrences of each character in the training file
map<char, int> alphabet;
// Characters that follow each pattern in the training file, plus in the generation if -t is used
map<string, vector<char> > map_patterns;

// Options
bool OPT_train=false;
bool OPT_lower=false;

// Number of guesses
int num_guess = 0;

int main(int argc, char** argv) {
    srand(time(0)); //! seed for random number generator -> aleatorio

    // Default values
    int c;
    string ini_str="";
    string train_file="";
    int num_char = 0;

    
    while ((c = getopt(argc, argv, "htls:f:n:")) != -1){
        switch(c){
            // help
            case 'h':
                printUsage(argv[0]);
                return 0;
            // initial string
            case 's': 
                ini_str=optarg;
                break;
            // train file
            case 'f': 
                train_file=optarg;
                break;
            // number of characters to generate
            case 'n':
                num_char = stoi(optarg);
                break;
            // allow training itself
            case 't':
                OPT_train=true;
                break;
            // allow only lower case letters
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

    string tmp_str = "";
    if (OPT_lower) {
        for (int x=0; x<ini_str.size(); x++){
            tmp_str += tolower(ini_str[x]);
        }

        ini_str = tmp_str;
    }

    cout << "Initial String: " << ini_str << endl;
    cout << "Train file: " << train_file << endl;
    cout << "N characters to generate: " << num_char << endl;
    cout << "Alow training itself: " << OPT_train << endl;
    cout << "Alow only lower case letters: " << OPT_lower << endl;
    cout << endl;
    cout << "Working..." << endl;
    cout << endl;


    firstPass(train_file,ini_str.size());

    cpm_gen(ini_str,ini_str.size(), num_char);

    cout << "Done!" << endl;
    cout << "Num guesses: " << num_guess << "/" << num_char << endl;
    cout << "Output file: out.txt" << endl;

    return 0;
}

// First pass through the training file to fill the alphabet and the map of patterns
void firstPass(string fileName,int k) {

    ifstream file(fileName);

    string pattern = "";
    char c = file.get();

    if (OPT_lower) {
        c = tolower(c);
    }
    
    while (!file.eof()) {
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

// Generation function, using the map of patterns obtained in the first pass
void cpm_gen(string ini_str,int k,int num_char) {

    ofstream OutFile("out.txt");
    
    OutFile << ini_str << "|";

    // Number of characters generated so far
    int char_counter = 0;
    // Pattern to be used in the next iteration
    string pattern = ini_str;

    // while there are characters to be generated
    while (char_counter<num_char) {

        char put_char;
        
        // if the pattern is not in the map, generate a random character
        if (map_patterns.count(pattern) == 0) { //padrao nao esta no map

            num_guess++;

            // get all possible characters
            double max = 0;
            int total_simbols = 0;
            for(std::map<char,int>::iterator it = alphabet.begin(); it != alphabet.end(); ++it) {
                total_simbols += it->second;
            }

            // generate a random character from the possible ones
            int random = rand() % total_simbols; // gera um numero aleatorio entre 0 e total_simbols-1

            int sum = 0;
            for(std::map<char,int>::iterator it = alphabet.begin(); it != alphabet.end(); ++it) {
                if (sum <= random && random < sum + alphabet[it->first]) {
                    put_char = it->first;
                    break;
                }
                sum += alphabet[it->first];
            }

            // if training is allowed, add the generated character to the map in the correct pattern
            if (OPT_train){
                map_patterns[pattern].push_back(put_char);     
            }

        // if the pattern is in the map, generate a character according to the distribution
        } else { 

            map <char, int> tmp;

            int total_simbols = 0;
            for (char c : map_patterns[pattern]) {

                if (c == '\0') {
                    continue;
                }

                tmp[c]++;
                total_simbols++;
            }

            // error check
            if (total_simbols == 0) { 
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

            // if training is allowed, add the generated character to the map in the correct pattern
            if (OPT_train){
                map_patterns[pattern].push_back(put_char);
            }
            
        }
        // write the generated character to the output file
        OutFile << put_char;

        // error check
        if (put_char == '\0') 
        {
            cout << "ERRO: put_char = \\0" << endl;
        }
        
        char_counter++;
        // update the pattern
        pattern.erase(0, 1); 
        pattern += put_char;

        if (OPT_train){
            alphabet.insert({put_char, 0});     
            alphabet[put_char]++;               
        }
    }
    OutFile.close();
}

void printUsage(string file) {
    cout << "Usage: " << file << " -s 'initial string' -f train_file.txt -n n_characters_generate [OPTIONS]" << endl;
    cout << "Example: " << file << " -s 'As the' -f othello.txt -n 2000" << endl;
    cout << "Options:" << endl;
    cout << "  -h  help" << endl;
    cout << "  -t  (allow training itself)" << endl;
    cout << "  -l  (allow only lower case letters - better for smaller input texts)" << endl;
}
