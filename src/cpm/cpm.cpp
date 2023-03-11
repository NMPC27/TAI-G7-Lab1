#include <fstream>
#include <algorithm>
#include "cpm.hpp"


bool StaticCopyPointerThreshold::surpassedThreshold(double hit_probability) {
    return hit_probability < static_threshold;
}

void StaticCopyPointerThreshold::reset() {}

bool DerivativeCopyPointerThreshold::surpassedThreshold(double hit_probability) {
    if (previous_hit_probability == -1)
        previous_hit_probability = hit_probability;

    double derivative = hit_probability - previous_hit_probability;
    previous_hit_probability = hit_probability;
    return derivative < derivative_threshold;
}

void DerivativeCopyPointerThreshold::reset() {
    previous_hit_probability = -1;
}

int RecentCopyPointerManager::newCopyPointer(std::vector<size_t> copy_pointers, int current_copy_pointer) {
    return copy_pointers.size() - 1;
}

int NextOldestCopyPointerManager::newCopyPointer(std::vector<size_t> copy_pointers, int current_copy_pointer) {
    return current_copy_pointer + 1;
}

void UniformDistribution::setBaseDistribution(std::map<char, int> histogram) {
    distribution.clear();

    for (auto pair : histogram)
        distribution[pair.first] = 1.0 / histogram.size();
}

void FrequencyDistribution::setBaseDistribution(std::map<char, int> histogram){
    distribution.clear();

    int total = 0;
    for (auto pair : histogram)
        total += pair.second;

    for (auto pair : histogram)
        distribution[pair.first] = (double) pair.second / total;
}

void CopyModel::initializeWithMostFrequent() {
    auto max_pair = std::max_element(alphabet_counts.begin(), alphabet_counts.end(),
            [](const std::pair<char, int>& x, const std::pair<char, int>& y) {return x.second < y.second;}
    );

    current_pattern = std::string(k-1, max_pair->first);
    current_pattern += reading_strategy->at(current_position);
}

void CopyModel::registerPattern() {
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

void CopyModel::advance() {
    current_pattern += reading_strategy->at(current_position++);
    current_pattern.erase(0, 1);
    
}

bool CopyModel::predict() {
    int predict_index = pointer_map[current_pattern].pointers[ pointer_map[current_pattern].copy_pointer_index ] + 1;

    prediction = reading_strategy->at(predict_index);
    actual = reading_strategy->at(current_position + 1);

    hit_probability = calculateProbability();

    bool hit = prediction == actual;

    if (hit) {
        pointer_map[current_pattern].hits++;
    } else {
        pointer_map[current_pattern].misses++;
    }

    // Check whether copy pointer should be changed
    if (pointer_threshold->surpassedThreshold(hit_probability)) {
        pointer_map[current_pattern].copy_pointer_index = pointer_manager->newCopyPointer(pointer_map[current_pattern].pointers, pointer_map[current_pattern].copy_pointer_index);

        pointer_map[current_pattern].hits = 0;
        pointer_map[current_pattern].misses = 0;
        pointer_threshold->reset();
    }

    // Update internal probability distribution
    setRemainderProbabilities(prediction, 1.0 - hit_probability);
    probability_distribution[prediction] = hit_probability;

    return hit;
}

void CopyModel::firstPass(std::string file_name) {
    
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

bool CopyModel::eof() {
    // We add one because we don't want to predict a character outside of the stream, so we end earlier
    return current_position + 1 >= reading_strategy->endOfStream();
}

int CopyModel::countOf(char c) {
    return alphabet_counts[c];
}

double CopyModel::calculateProbability() {
    int hits = pointer_map[current_pattern].hits;
    int misses = pointer_map[current_pattern].misses; 
    return (hits + alpha) / (hits + misses + 2 * alpha);
}

void CopyModel::setRemainderProbabilities(char exception, double probability_to_distribute) {
    double base_remainder_total = 0.0;
    for (auto pair : base_distribution->distribution)
        if (pair.first != exception)
            base_remainder_total += pair.second;
    
    for (auto pair : base_distribution->distribution)
        if (pair.first != exception)
            probability_distribution[pair.first] = probability_to_distribute * base_distribution->distribution[pair.first] / base_remainder_total;
}