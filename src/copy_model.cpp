#include <fstream>
#include "copy_model.hpp"

bool StaticCopyPointerThreshold::surpassedThreshold(double hit_probability) {
    return hit_probability < 0.5;
}

int RecentCopyPointerManager::newCopyPointer(std::vector<size_t> copy_pointers, int current_copy_pointer) {
    return current_copy_pointer + 1;
}

bool CopyModel::predictNext() {
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

}

bool CopyModel::eof() {
    return current_position >= reading_strategy->end_of_stream();
}

void CopyModel::reset() {
    current_position = 0;
    alphabet_counts.clear();
    pointer_map.clear();
}

double CopyModel::calculateProbability() {
    int hits = pointer_map[current_pattern].hits;
    int misses = pointer_map[current_pattern].misses; 
    return (hits + alpha) / (hits + misses + 2 * alpha);
}
