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


int main(int argc, char** argv) {

    // Defaults
    ReadingStrategy* reading_strategy = new InMemoryReadingStrategy();
    CopyPointerThreshold* pointer_thresholds = new StaticCopyPointerThreshold(0.5);
    CopyPointerManager* pointer_manager = new NextOldestCopyPointerManager();
    BaseDistribution* base_distribution = new FrequencyDistribution();

    CopyModel model = CopyModel(1, 1, reading_strategy, &pointer_thresholds, 1, pointer_manager, base_distribution);

    string file_name = string(argv[optind]);

    struct stat file_status;
    stat(file_name.c_str(), &file_status);
    if (errno == ENOENT) {
        cout << "Error: file '" << file_name << "' doesn't exist!" << endl;
        return 1;
    }

    model.firstPass(file_name);

    delete reading_strategy;
    delete pointer_thresholds;
    delete pointer_manager;
    delete base_distribution;

    int sum = 0;
    for (map<char,double>::iterator it = model.probability_distribution.begin(); it != model.probability_distribution.end(); ++it) {
        sum += model.countOf(it->first);
    }
    double entropy = 0.0;
    for (map<char,double>::iterator it = model.probability_distribution.begin(); it != model.probability_distribution.end(); ++it) {
        double prob = (double) model.countOf(it->first) / sum;
        entropy += -log2(prob) * prob;
    }
    cout << "Entropy: " << entropy << " bits" << endl;

    return 0;
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
