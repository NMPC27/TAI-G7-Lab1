#include <iostream>
#include <fstream>
#include <map>
#include <algorithm>
#include <string>
#include <list>
#include <vector>
#include <cmath>
#include "unistd.h"
#include "cpm.h"

using namespace std;

vector<int> arr_file;
map<char, int> alphabet;
int count_alphabet = 0;
map<char, double> base_distribution;


int main(int argc, char** argv) {

    int c;
    int k = 4;
    double alpha = 0.1;
    while ((c = getopt(argc, argv, "hbk:a:p:r:t:")) != -1){
        switch(c){
            case 'h':
                printUsage(argv[0]);
                printOptions();
                return 0;
            case 'k':
                k = stoi(optarg);
                break;
            case 'a':
                alpha = stof(optarg);
                break;
            case 'p': //! NOT DONE
                if (optarg[0] == 'u') {
                    //base_distribution = "uniform_distribution"; 
                } else if (optarg[0] == 'd') {
                    //base_distribution = "default_distribution";
                } else {
                    cout << "Invalid option for -p" << endl;
                    return 1;
                }
                break;
            case 'b': //! NOT DONE - ler file em binario
                break;
            case 'r': //! NOT DONE
                if (optarg[0] == 'o') {
                    //copy_pointer_reposition = "oldest";
                } else if (optarg[0] == 'n') {
                    //copy_pointer_reposition = "newer";
                } else {
                    cout << "Invalid option for -r" << endl;
                    return 1;
                }
                break;
            
            case 't':
                {
                    string optarg_string = string(optarg);

                    int pos = optarg_string.find(":");
                    if (pos == -1) {
                        cout << "Invalid option for -t: " << optarg << endl;
                        return 1;
                    }

                    string opt = optarg_string.substr(0, pos);
                    string value = optarg_string.substr(pos+1, optarg_string.length());

                    if (opt == "n") {
                        //threshold = "static_probability";
                        //threshold_value = stof(value);
                    } else if (opt == "f") {
                        //threshold = "number_of_successive_fails";
                        //threshold_value = stoi(value);
                    } else if (opt == "c") {
                        //threshold = "absolute_value_of_the_negative_derivative";
                        //threshold_value = stof(value);
                    } else {
                        cout << "Invalid option for -t: " << optarg << endl;
                        return 1;
                    }
                }

                break;                

            case '?':
                printUsage(argv[0]);
                return 1;
        }
    }

    string fileName = string(argv[optind]);

    firstPass(fileName);

    cpm(k, alpha);

    return 0;
}

void firstPass(string fileName) {

    ifstream file(fileName);

    char c = file.get();
    
    while (!file.eof()) {
        count_alphabet++;

        arr_file.push_back((int) c);

        alphabet.insert({c, 0});
        alphabet[c]++;

        c = file.get();
    }

    file.close();
}

struct PatternInfo {
    vector<size_t> pointers;
    int copy_pointer_index;     // Pattern's last symbol
    int hits;
    int misses;
};

void cpm(int k, double alpha) {

    char c;

    map<string, PatternInfo> pointer_map; // map dos padroes, lista das posições onde forem encontradas

    string pattern = "";

    // Base distribution depends on the symbol's relative frequencies
    for (auto pair : alphabet) {
        base_distribution[pair.first] = (double) pair.second / count_alphabet;
    }
    
    printAlphabet(base_distribution);

    // Handle the first k-1 symbols
    for (int i = 0; i < k-1; i++)
    {
        pattern += arr_file[i];
    }
    
    double information_sum;

    for (size_t pos = k-1; pos < arr_file.size(); pos++)
    {
        c = arr_file[pos];//le o caracter

        pattern += c; // adiciona o caracter ao padrao

        if (pointer_map.count(pattern) == 0) { //padrao nao esta no map

            struct PatternInfo pattern_info = {
                {pos}, 0, 0, 0
            };

            pointer_map.insert({pattern, pattern_info}); // insere o padrao no map com a posição atual

        } else { //padrao ja esta no map

            pointer_map[pattern].pointers.push_back(pos);// adiciona a posição atual ao padrao

            // Predict
            int predict_index = pointer_map[pattern].pointers[pointer_map[pattern].copy_pointer_index] + 1;

            // caracteres: previsão e o verdadeiro
            int prediction = arr_file[predict_index];
            int actual = arr_file[pos + 1];

            if (prediction == actual) {
                pointer_map[pattern].hits++;

                printf("Hit: %s, Prediction: %c\n", pattern.c_str(), prediction);
            } else {
                pointer_map[pattern].misses++;

                printf("Miss: %s, Prediction: %c, Actual: %c\n", pattern.c_str(), prediction, actual);
            }

            double hit_probability = calculateProbability(pointer_map[pattern].hits, pointer_map[pattern].misses, alpha);
            
            //! Calculate information of current character  ???
            information_sum += -log2((prediction == actual) ? hit_probability : ((1 - hit_probability) / (alphabet.size() - 1)));

            // Check whether copy pointer should be changed
            if (hit_probability < 0.5) {
                pointer_map[pattern].copy_pointer_index++;

                pointer_map[pattern].hits = 0;
                pointer_map[pattern].misses = 0;

                printf("Hit probability below threshold, choosing copy pointer %d\n", pointer_map[pattern].copy_pointer_index);
            }

        }


        pattern.erase(0, 1); // remove o primeiro caracter do padrao

    }

    cout << "Amount of information: " << information_sum << endl;

    //printMap(map);
    //printAlphabet(alphabet);
}

double calculateProbability(int hits, int misses, double alpha) {
    return (hits + alpha) / (hits + misses + 2 * alpha);
}

void printAlphabet(map<char, double> pointer_map) {

    for (auto it = pointer_map.begin(); it != pointer_map.end(); it++) {
        cout << it->first << " : " << it->second << endl;
    }
}

void outputProbabilityDistribution(double hit_probability) {

}

void printUsage(char* prog_name) {
    cout << "Usage: " << prog_name << " [OPTIONS] file" << endl;
}

void printOptions() {
    cout << "Options:" << endl;
    cout << "\t-h\t\tShow this help message" << endl;
    cout << "\t-k K\t\tSize of the sliding window (default: 4)" << endl;
    cout << "\t-a A\t\tSmoothing parameter alpha for the prediction probability (default: 0.1)" << endl;
    cout << "\t-p P\t\tProbability distribution of the characters other than the one being predicted (default: d):" << endl;
    cout << "\t\t\t\tu - uniform distribution" << endl;
    cout << "\t\t\t\td - distribution based on the symbols' relative frequencies" << endl;
    cout << "\t-b\t\tRead file in binary" << endl;
    cout << "\t-r R\t\tCopy pointer reposition (default: o):" << endl;
    cout << "\t\t\t\to - oldest" << endl;
    cout << "\t\t\t\tn - newer" << endl;
    cout << "\t-t T\t\tThreshold for copy pointer switch (default: n:0.50):" << endl;
    cout << "\t\t\t\tn:X - static probability below X" << endl;
    cout << "\t\t\t\tf:X - number of successive fails above X" << endl; //! temos de ver que o numero faz sentido
    cout << "\t\t\t\tc:X - absolute value of the negative derivative of the prediction probability above X" << endl;
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