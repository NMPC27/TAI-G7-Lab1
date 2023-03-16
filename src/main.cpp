#include <iostream>
#include <fstream>
#include <map>
#include <algorithm>
#include <string>
#include <list>
#include <vector>
#include <cmath>
#include "unistd.h"
#include "cpm/cpm.hpp"
#include "main.hpp"

using namespace std;


int main(int argc, char** argv) {

    int c;
    int k = 4;
    double alpha = 0.1;
    bool verbose = false;
    ReadingStrategy* reading_strategy = nullptr;
    CopyPointerThreshold* pointer_threshold = nullptr;
    CopyPointerManager* pointer_manager = nullptr;
    BaseDistribution* base_distribution = nullptr;

    while ((c = getopt(argc, argv, "hvbk:a:p:r:t:")) != -1){
        switch(c){
            case 'h':
                printUsage(argv[0]);
                printOptions();
                return 0;

            case 'v':
                verbose = true;
                break;

            case 'k':
                k = stoi(optarg);
                break;

            case 'a':
                alpha = stof(optarg);
                break;

            case 'p':
                if (optarg[0] == 'u') {
                    base_distribution = new UniformDistribution(); 
                } else if (optarg[0] == 'f') {
                    base_distribution = new FrequencyDistribution();
                } else {
                    cout << "Error: invalid option for '-p' (" << optarg[0] << ")" << endl;
                    return 1;
                }
                break;

            case 'b':
                cout << "Error: '-b' option currently not supported" << endl;
                return 1;
                break;

            case 'r':
                if (optarg[0] == 'o') {
                    pointer_manager = new NextOldestCopyPointerManager();
                } else if (optarg[0] == 'n') {
                    pointer_manager = new RecentCopyPointerManager();
                } else {
                    cout << "Error: invalid option for '-r' (" << optarg[0] << ")" << endl;
                    return 1;
                }
                break;
            
            case 't':
                {
                    string optarg_string = string(optarg);

                    int pos = optarg_string.find(":");
                    if (pos == -1) {
                        cout << "Error: invalid option for '-t' (" << optarg << ")" << endl;
                        return 1;
                    }

                    string opt = optarg_string.substr(0, pos);
                    string value = optarg_string.substr(pos+1, optarg_string.length());

                    if (opt == "n") {
                        double threshold_value = stof(value);
                        pointer_threshold = new StaticCopyPointerThreshold(threshold_value);
                    } else if (opt == "f") {
                        int threshold_value = stoi(value);
                        if (threshold_value > 0){
                            pointer_threshold = new SuccessFailsCopyPointerThreshold(threshold_value);
                        }else{
                            cout << "Error: invalid option for '-t f:X' (" << optarg << ")" << endl;
                            return 1;
                        }
                        //cout << "Error: '-t f:X' option currently not supported" << endl;
                        
                    } else if (opt == "c") {
                        double threshold_value = stof(value);
                        pointer_threshold = new DerivativeCopyPointerThreshold(threshold_value);
                    } else {
                        cout << "Error: invalid option for '-t' (" << optarg << ")" << endl;
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
        printUsage(argv[0]);
        cout << "Error: no file was specified!" << endl;
        return 1;
    }

    // Defaults
    if (reading_strategy == nullptr) reading_strategy = new InMemoryReadingStrategy();
    if (pointer_threshold == nullptr) pointer_threshold = new StaticCopyPointerThreshold(0.5);
    if (pointer_threshold == nullptr) pointer_threshold = new SuccessFailsCopyPointerThreshold(3);
    if (pointer_manager == nullptr) pointer_manager = new NextOldestCopyPointerManager();
    if (base_distribution == nullptr) base_distribution = new FrequencyDistribution();

    CopyModel model = CopyModel(k, alpha, reading_strategy, pointer_threshold, pointer_manager, base_distribution);

    string fileName = string(argv[optind]);
    model.firstPass(fileName);

    map<char, double> information_sums;

    model.initializeWithMostFrequent();
    while (!model.eof()) {
        bool exists = model.registerPattern();

        int output_color_condition = exists ? 1 : 0;
        if (exists) {
            bool hit = model.predict();
            output_color_condition += hit ? 1 : 0;
        // TODO: should we perform guesses like this? or "predict"?
        } else {
            model.guess();
        }
        model.advance();

        // The probability distribution that the model provides doesn't account for whether or not the current prediction was a success,
        // as that would incorporate information from the future which would not be known to the decoder.
        if (verbose) {
            string output_color;
            switch (output_color_condition) {
                // New pattern, doesn't exist
                case 0:
                    output_color = "\e[0;33m";
                    break;
                // Pattern exists, but no hit
                case 1:
                    output_color = "\e[0;31m";
                    break;
                // Pattern exists, with hit
                case 2:
                    output_color = "\e[0;32m";
                    break;
            }
            cout << output_color;
            outputProbabilityDistribution(model.prediction, model.hit_probability, model.probability_distribution);
            cout << "\e[0m";
        }
        information_sums[model.actual] += -log2(model.probability_distribution[model.actual]);
    }

    delete reading_strategy;
    delete pointer_threshold;
    delete pointer_manager;
    delete base_distribution;

    double information_sum = 0.0;
    cout << "Average amount of information in symbol..." << endl;
    for (auto pair : information_sums) {
        cout << pair.first << ": " << pair.second / model.countOf(pair.first) << " bits" << endl;
        information_sum += pair.second;
    }
    cout << "Total amount of information: " << information_sum << " bits" << endl;

    return 0;
}

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
    cout << "\t-v\t\tVerbose output (output probability distribution at each encoding step)" << endl;
    cout << "\t-k K\t\tSize of the sliding window (default: 4)" << endl;
    cout << "\t-a A\t\tSmoothing parameter alpha for the prediction probability (default: 0.1)" << endl;
    cout << "\t-p P\t\tProbability distribution of the characters other than the one being predicted (default: f):" << endl;
    cout << "\t\t\t\tu - uniform distribution" << endl;
    cout << "\t\t\t\tf - distribution based on the symbols' relative frequencies" << endl;
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