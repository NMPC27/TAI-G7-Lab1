#include <iostream>
#include <fstream>
#include <map>
#include <algorithm>
#include <string>
#include <list>
#include <vector>
#include <cmath>
#include <sys/stat.h>
#include <unistd.h>
#include <chrono>

#include "cpm/cpm.hpp"
#include "main.hpp"

using namespace std;

/** @brief Maximum number of unique copy pointers */
#define POINTER_THRESHOLD_MAX_NUMBER 3
#define POINTER_THRESHOLD_MASK_STATIC 1
#define POINTER_THRESHOLD_MASK_DERIVATIVE 2
#define POINTER_THRESHOLD_MASK_SUCCESSIVE_FAILS 4


int main(int argc, char** argv) {

    chrono::steady_clock sc;   // create an object of `steady_clock` class
    auto start = sc.now();     // start timer

    int c;
    int k = 4;
    double alpha = 0.1;
    enum VerboseMode{human, machine, progress, none} verbose_mode = VerboseMode::none;
    ReadingStrategy* reading_strategy = nullptr;
    CopyPointerThreshold* pointer_thresholds[POINTER_THRESHOLD_MAX_NUMBER];
    int pointer_threshold_number = 0;
    int pointer_threshold_mask = 0;
    CopyPointerManager* pointer_manager = nullptr;
    BaseDistribution* base_distribution = nullptr;

    while ((c = getopt(argc, argv, "hv:bk:a:p:r:t:")) != -1){
        switch(c){
            case 'h':
                printUsage(argv[0]);
                printOptions();
                return 0;

            case 'v':
                switch (optarg[0]) {
                    case 'h':
                        verbose_mode = VerboseMode::human;
                        break;
                    case 'm':
                        verbose_mode = VerboseMode::machine;
                        break;
                    case 'p':
                        verbose_mode = VerboseMode::progress;
                        break;
                    default:
                        cout << "Error: invalid option for '-v' (" << optarg[0] << ")" << endl;
                        return 1;
                }
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

            case 'r':
                {
                string optarg_string = string(optarg);
                int pos = optarg_string.find(":");

                string opt = optarg_string.substr(0, pos);
                string value = optarg_string.substr(pos+1, optarg_string.length());

                if (opt == "o") {
                    pointer_manager = new NextOldestCopyPointerManager();
                } else if (opt == "n") {
                    pointer_manager = new RecentCopyPointerManager();
                } else if (opt == "m") {
                    pointer_manager = new MostCommonCopyPointerManager();
                } else if (opt == "c"){

                    int circular_value = stoi(value);
                    if (circular_value > 0){
                        pointer_manager = new CircularArrayCopyPointerManager(circular_value);
                    }else{
                        cout << "Error: invalid option for '-r c:X' (" << optarg << ")" << endl;
                        return 1;
                    }
                }else {
                    cout << "Error: invalid option for '-r' (" << optarg[0] << ")" << endl;
                    return 1;
                }
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
                        if (pointer_threshold_mask & POINTER_THRESHOLD_MASK_STATIC) {
                            cout << "Error: mode '" << opt << "' for option '-t' was specified more than once (repeated value '" << optarg << "')" << endl;
                            return 1;
                        }
                        double threshold_value = stof(value);
                        pointer_thresholds[pointer_threshold_number] = new StaticCopyPointerThreshold(threshold_value);
                        pointer_threshold_number++;
                        pointer_threshold_mask |= POINTER_THRESHOLD_MASK_STATIC;
                    } else if (opt == "f") {
                        if (pointer_threshold_mask & POINTER_THRESHOLD_MASK_SUCCESSIVE_FAILS) {
                            cout << "Error: mode '" << opt << "' for option '-t' was specified more than once (repeated value '" << optarg << "')" << endl;
                            return 1;
                        }
                        int threshold_value = stoi(value);
                        if (threshold_value > 0){
                            pointer_thresholds[pointer_threshold_number] = new SuccessFailsCopyPointerThreshold(threshold_value);
                            pointer_threshold_number++;
                            pointer_threshold_mask |= POINTER_THRESHOLD_MASK_SUCCESSIVE_FAILS;
                        }else{
                            cout << "Error: invalid option for '-t f:X' (" << optarg << ")" << endl;
                            return 1;
                        }
                        //cout << "Error: '-t f:X' option currently not supported" << endl;
                        
                    } else if (opt == "c") {
                        if (pointer_threshold_mask & POINTER_THRESHOLD_MASK_DERIVATIVE) {
                            cout << "Error: mode '" << opt << "' for option '-t' was specified more than once (repeated value '" << optarg << "')" << endl;
                            return 1;
                        }
                        double threshold_value = stof(value);
                        pointer_thresholds[pointer_threshold_number] = new DerivativeCopyPointerThreshold(threshold_value);
                        pointer_threshold_number++;
                        pointer_threshold_mask |= POINTER_THRESHOLD_MASK_DERIVATIVE;
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
    if (pointer_threshold_number == 0) {
        pointer_thresholds[pointer_threshold_number] = new StaticCopyPointerThreshold(0.5);
        pointer_threshold_number++;
    }
    if (pointer_manager == nullptr) pointer_manager = new NextOldestCopyPointerManager();
    if (base_distribution == nullptr) base_distribution = new FrequencyDistribution();

    CopyModel model = CopyModel(k, alpha, reading_strategy, pointer_thresholds, pointer_threshold_number, pointer_manager, base_distribution);

    string file_name = string(argv[optind]);

    struct stat file_status;
    stat(file_name.c_str(), &file_status);
    if (errno == ENOENT) {
        cout << "Error: file '" << file_name << "' doesn't exist!" << endl;
        return 1;
    }

    model.firstPass(file_name);

    map<char, double> information_sums;

    model.initializeWithMostFrequent();

    if (verbose_mode == VerboseMode::machine)
        outputProbabilityDistributionCSVheader();

    while (!model.eof()) {
        bool pattern_has_past = model.registerPattern();
        bool can_predict = model.predictionSetup(pattern_has_past);

        int output_color_condition = can_predict ? 1 : 0;
        if (can_predict) {
            bool hit = model.predict();
            output_color_condition += hit ? 1 : 0;
        } else {
            model.guess();
        }
        model.advance();

        // The probability distribution that the model provides doesn't account for whether or not the current prediction was a success,
        // as that would incorporate information from the future which would not be known to the decoder.
        switch (verbose_mode) {
            case VerboseMode::human:
                {
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
                    outputProbabilityDistributionHuman(model.prediction, model.actual, model.hit_probability, model.probability_distribution);
                    cout << "\e[0m";
                }
                break;
            case VerboseMode::machine:
                outputProbabilityDistributionCSVbody(model.prediction, model.actual, model.hit_probability, model.probability_distribution);
                break;
            case VerboseMode::progress:
                printf("Progress: %3f%%\r", model.progress() * 100);
                break;
            case VerboseMode::none:
                break;
        }
        information_sums[model.actual] += -log2(model.probability_distribution[model.actual]);
    }

    delete reading_strategy;
    for (int i = 0; i < pointer_threshold_number; i++)
        delete pointer_thresholds[i];
    delete pointer_manager;
    delete base_distribution;

    double information_sum = 0.0;
    cout << "Average amount of information in symbol..." << endl;
    for (auto pair : information_sums) {
        cout << pair.first << ": " << pair.second / model.countOf(pair.first) << " bits" << endl;
        information_sum += pair.second;
    }

    int sum=0;
    for(std::map<char,double>::iterator it = model.probability_distribution.begin(); it != model.probability_distribution.end(); ++it) {
        sum+=model.countOf(it->first);
    }
    cout << "Mean amount of information of a symbol: " << information_sum/sum << " bits" << endl;
    cout << "Total amount of information: " << information_sum << " bits" << endl;

    auto end = sc.now();
    auto time_span = static_cast<chrono::duration<double>>(end - start);   // measure time span between start & end

    cout << "Time elapsed: " << time_span.count() << " seconds" << endl;

    return 0;
}

void outputProbabilityDistributionHuman(char prediction, char actual, double hit_probability, map<char, double> base_distribution) {
    cout << "Prediction: '" << prediction << "', Actual: '" << actual << "', " << hit_probability << "\t" << " | Distribution: ";
    for (auto pair : base_distribution) {
        cout << "('" << pair.first << "', " << pair.second << ") ";
    }
    cout << endl;
}

void outputProbabilityDistributionCSVheader() {
    cout << "Prediction, Actual, Hit probability, Distribution" << endl;
}

void outputProbabilityDistributionCSVbody(char prediction, char actual, double hit_probability, map<char, double> distribution) {
    cout << prediction << "," << actual << "," << hit_probability << ",";
    for (auto pair : distribution) {
        cout << pair.first << ":" << pair.second << ":";
    }
    cout << endl;
}

void printUsage(char* prog_name) {
    cout << "Usage: " << prog_name << " [OPTIONS] file" << endl;
}

void printOptions() {
    cout << "Options:" << endl;
    cout << "\t-h\t\tShow this help message" << endl;
    cout << "\t-v V\t\tAdditional output (verbose modes output the probability distribution at each encoding step):" << endl;
    cout << "\t\t\t\th - Human-readable verbose output, color-coded depending on whether a hit/miss/guess occurred" << endl;
    cout << "\t\t\t\tm - Machine-readable verbose output, without color-coding and minimal flair (CSV format with header)" << endl;
    cout << "\t\t\t\tp - Print the progress of processing the sequence" << endl;
    cout << "\t-k K\t\tSize of the sliding window (default: 4)" << endl;
    cout << "\t-a A\t\tSmoothing parameter alpha for the prediction probability (default: 0.1)" << endl;
    cout << "\t-p P\t\tProbability distribution of the characters other than the one being predicted (default: f):" << endl;
    cout << "\t\t\t\tu - uniform distribution" << endl;
    cout << "\t\t\t\tf - distribution based on the symbols' relative frequencies" << endl;
    cout << "\t-r R\t\tCopy pointer reposition (default: o):" << endl;
    cout << "\t\t\t\to - oldest" << endl;
    cout << "\t\t\t\tn - newer" << endl;
    cout << "\t\t\t\tm - most common prediction among all pointers" << endl;
    cout << "\t-t T\t\tThreshold for copy pointer switch (default: n:0.50):" << endl;
    cout << "\t\t\t\tn:X - static probability below X" << endl;
    cout << "\t\t\t\tf:X - number of successive fails above X" << endl; //! temos de ver que o numero faz sentido
    cout << "\t\t\t\tc:X - absolute value of the negative derivative of the prediction probability above X" << endl;
}
