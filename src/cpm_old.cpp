#include <fstream>
#include <algorithm>
#include "cpm.hpp"
#include "cpm_old.hpp"


void CopyModelOld::initializeWithMostFrequent() {
    auto max_pair = std::max_element(alphabet_counts.begin(), alphabet_counts.end(),
            [](const std::pair<char, int>& x, const std::pair<char, int>& y) {return x.second < y.second;}
    );

    current_pattern = std::string(k-1, max_pair->first);
    current_pattern += reading_strategy->at(current_position);
}

void CopyModelOld::registerPattern() {
    if (pointer_map.count(current_pattern) == 0) {

        struct PatternInfo pattern_info = {
            .pointers = {current_position},
            .copy_pointer_index = 0,
            .hits = 0,
            .misses = 0
        };

        pointer_map.insert({current_pattern, pattern_info});

    }
    else {
        pointer_map[current_pattern].pointers.push_back(current_position);
    }
}

void CopyModelOld::advance() {
    current_pattern += reading_strategy->at(current_position++);
    current_pattern.erase(0, 1);
}

bool CopyModelOld::predict() {
    int predict_index = pointer_map[current_pattern].pointers[ pointer_map[current_pattern].copy_pointer_index ] + 1;

    prediction = reading_strategy->at(predict_index);
    char actual = reading_strategy->at(current_position + 1);

    bool hit = prediction == actual;

    if (hit) {
        pointer_map[current_pattern].hits++;
    } else {
        pointer_map[current_pattern].misses++;
    }

    hit_probability = calculateProbability();

    // Check whether copy pointer should be changed
    if (pointer_threshold->surpassedThreshold(hit_probability)) {
        pointer_map[current_pattern].copy_pointer_index = pointer_manager->newCopyPointer(pointer_map[current_pattern].pointers, pointer_map[current_pattern].copy_pointer_index);

        pointer_map[current_pattern].hits = 0;
        pointer_map[current_pattern].misses = 0;
    }

    // Update internal probability distribution
    setRemainderProbabilities(prediction, 1.0 - hit_probability);
    probability_distribution[prediction] = hit_probability;

    return hit;
}

void CopyModelOld::firstPass(std::string file_name) {
    
    std::ifstream file(file_name);

    char c = file.get();
    
    while (!file.eof()) {
        reading_strategy->read(c);

        alphabet_counts.insert({c, 0});
        alphabet_counts[c]++;

        c = file.get();
    }

    file.close();

    base_distribution->setBaseDistribution(alphabet_counts);
    probability_distribution = std::map<char, double>(base_distribution->distribution);
}

bool CopyModelOld::eof() {
    return current_position >= reading_strategy->end_of_stream();
}

void CopyModelOld::reset() {
    current_position = 0;
    alphabet_counts.clear();
    pointer_map.clear();
}

double CopyModelOld::calculateProbability() {
    int hits = pointer_map[current_pattern].hits;
    int misses = pointer_map[current_pattern].misses; 
    return (hits + alpha) / (hits + misses + 2 * alpha);
}

void CopyModelOld::setRemainderProbabilities(char exception, double probability_to_distribute) {
    double base_remainder_total = 0.0;
    for (auto pair : base_distribution->distribution)
        if (pair.first != exception)
            base_remainder_total += pair.second;
    
    for (auto pair : base_distribution->distribution)
        if (pair.first != exception)
            probability_distribution[pair.first] = probability_to_distribute * base_distribution->distribution[pair.first] / base_remainder_total;
}