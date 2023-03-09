#include <iostream>
#include <fstream>
#include <map>
#include <algorithm>
#include <string>
#include <list>
#include <vector>
#include <cmath>
#include "unistd.h"
#include "parser.hpp"
#include "copy_model.hpp"
#include "cpm.hpp"

using namespace std;

vector<int> arr_file;
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

    // TODO: obtain from passed arguments
    ReadingStrategy reading_strategy;
    CopyPointerThreshold pointer_threshold;
    CopyPointerManager pointer_manager;

    CopyModel model = CopyModel(alpha, &reading_strategy, &pointer_threshold, &pointer_manager);

    string fileName = string(argv[optind]);
    model.firstPass(fileName);

    while (!model.eof()) {
        model.predictNext();

        outputProbabilityDistribution(model.prediction, model.hit_probability, model.probability_distribution);
    }

    return 0;
}

void cpm(int k, double alpha) {

    char c;

    map<string, PatternInfo> pointer_map;

    string pattern = "";

    // Base distribution depends on the symbol's relative frequencies
    for (auto pair : alphabet_count)
    {
        base_distribution[pair.first] = (double) pair.second / count_alphabet;
    }
    
    printAlphabet(base_distribution);

    // Handle the first k-1 symbols
    for (int i = 0; i < k-1; i++)
    {
        pattern += arr_file[i];
    }
    
    double information_sum;

    CopyModel model;

    for (size_t pos = k-1; pos < arr_file.size(); pos++)
    {
        c = arr_file[pos];

        pattern += c;

        if (pointer_map.count(pattern) == 0) {

            struct PatternInfo pattern_info = {
                .pointers = {pos},
                .copy_pointer_index = 0,
                .hits = 0,
                .misses = 0
            };

            pointer_map.insert({pattern, pattern_info});

        } else {

            pointer_map[pattern].pointers.push_back(pos);

            model.predictNext()

        }


        pattern.erase(0, 1);

    }

}

double calculateProbability(int hits, int misses, double alpha) {
    return (hits + alpha) / (hits + misses + 2 * alpha);
}

// TODO: What should the model provide?
void outputProbabilityDistribution(char prediction, double hit_probability, map<char, double> base_distribution) {

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

void printAlphabet(map<char, double> pointer_map) {

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