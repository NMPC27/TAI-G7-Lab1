#include <iostream>
#include <string>
#include <sstream>
#include <random>
#include "unistd.h"
#include "cpm.hpp"
#include "cpm_gen.hpp"

using namespace std;


int main(int argc, char** argv) {

    int c;
    int k = 4;
    double alpha = 0.1;
    ReadingStrategy* reading_strategy;
    CopyPointerThreshold* pointer_threshold;
    CopyPointerManager* pointer_manager;
    BaseDistribution* base_distribution;

    // TODO: command-line parsing, same from main

    if (optind == argc) {
        cout << "Error: no file was specified!" << endl;
        return 1;
    }

    // TODO: obtain from passed argsuments
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
    }

    cout << "Predictions:" << endl;

    unsigned int number_of_symbols_to_predict;
    string input_line;
    mt19937 gen;
    gen.seed(1);
    uniform_real_distribution<> dis(0.0, 1.0);

    getline(cin, input_line);
    stringstream(input_line) >> number_of_symbols_to_predict;
    while (number_of_symbols_to_predict > 0) {
        for (unsigned int i = number_of_symbols_to_predict; i > 0; i--) {
            model.registerPattern();
            model.predict();
            
            double random_number = dis(gen);
            char selected_symbol;
            double cummulative_sum = 0.0;
            for (auto pair : model.probability_distribution) {
                if (random_number < cummulative_sum)
                    break;
                selected_symbol = pair.first;
                cummulative_sum += pair.second;
            }

            reading_strategy->appendPrediction(selected_symbol);
            cout << selected_symbol;

            model.advance();
        }

        getline(cin, input_line);
        stringstream(input_line) >> number_of_symbols_to_predict;
    }

    return 0;
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
    cout << "\t\t\t\tf:X - number of successive fails above X" << endl;
    cout << "\t\t\t\tc:X - absolute value of the negative derivative of the prediction probability above X" << endl;
}