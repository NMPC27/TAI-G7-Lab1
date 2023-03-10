#include <iostream>
#include <fstream>
#include <map>
#include <algorithm>
#include <string>
#include <list>
#include <vector>
#include <cmath>
#include "unistd.h"
#include "cpm.hpp"
#include "cpm_main.hpp"

using namespace std;


int main(int argc, char** argv) {

    int c;
    int k = 4;
    double alpha = 0.1;
    ReadingStrategy* reading_strategy;
    CopyPointerThreshold* pointer_threshold;
    CopyPointerManager* pointer_manager;
    BaseDistribution* base_distribution;

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

    if (optind == argc) {
        cout << "Error: no file was specified!" << endl;
        return 1;
    }

    // TODO: obtain from passed arguments
    InMemoryReadingStrategy imrs = InMemoryReadingStrategy();
    StaticCopyPointerThreshold scpt = StaticCopyPointerThreshold();
    RecentCopyPointerManager rcpm = RecentCopyPointerManager();
    UniformDistribution ud = UniformDistribution();
    reading_strategy = &imrs;
    pointer_threshold = &scpt;
    pointer_manager = &rcpm;
    base_distribution = &ud;

    CopyModel model = CopyModel(k, alpha, reading_strategy, pointer_threshold, pointer_manager, base_distribution);

    string fileName = string(argv[optind]);
    model.firstPass(fileName);

    model.initializeWithMostFrequent();
    while (!model.eof()) {
        model.registerPattern();
        model.predict();
        model.advance();
        outputProbabilityDistribution(model.prediction, model.hit_probability, model.probability_distribution);
    }

    return 0;
}

// TODO: What should the model provide, and how?
void outputProbabilityDistribution(char prediction, double hit_probability, map<char, double> base_distribution) {
    cout << "Prediction: '" << prediction << "', " << hit_probability << " | Distribution: ";
    for (auto pair : base_distribution) {
        cout << "('" << pair.first << "', " << pair.second << ") ";
    }
    cout << endl;
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