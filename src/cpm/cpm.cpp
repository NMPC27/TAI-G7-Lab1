#include <fstream>
#include <algorithm>
#include <list>
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

bool SuccessFailsCopyPointerThreshold::surpassedThreshold(double hit_probability) {
    if (previous_hit_probability == -1)
        previous_hit_probability = hit_probability;

    bool fail = hit_probability < previous_hit_probability;
    if (fail) fails_count++;
    else fails_count = fails_count > 0 ? fails_count - 1 : 0;

    return fails_count >= fails_threshold;
}

void SuccessFailsCopyPointerThreshold::reset() {
    previous_hit_probability = -1;
    fails_count = 0;
}


bool SimpleCopyPointerManager::registerCopyPointer(std::string pattern, size_t position) {
    if (pointer_map.count(pattern) == 0) {

        struct SimplePointerInfo pattern_info = {
            .pointers = {position},
            .copy_pointer_index = 0,
        };

        pointer_map.insert({pattern, pattern_info});
        
        return false;
    }
    
    pointer_map[pattern].pointers.push_back(position);
    return true;
}

int SimpleCopyPointerManager::getCopyPointer(std::string pattern) {
    return pointer_map[pattern].pointers[pointer_map[pattern].copy_pointer_index];
}

void SimpleCopyPointerManager::reportPrediction(std::string pattern, bool hit) {
    if (hit) {
        hits++;
    } else {
        misses++;
    }
}

void SimpleCopyPointerManager::reset() {
    hits = 0;
    misses = 0;
}

int SimpleCopyPointerManager::getHits(std::string current_pattern) { return hits; }

int SimpleCopyPointerManager::getMisses(std::string current_pattern) { return misses; }

void RecentCopyPointerManager::repositionCopyPointer(std::string pattern, ReadingStrategy* reading_strategy) {
    // second to last copy pointer (because most recent could lead to predicting future)
    pointer_map[pattern].copy_pointer_index = pointer_map[pattern].pointers.size() - 2;
}

void NextOldestCopyPointerManager::repositionCopyPointer(std::string pattern, ReadingStrategy* reading_strategy) {
    pointer_map[pattern].copy_pointer_index += 1;
}

void MostCommonCopyPointerManager::repositionCopyPointer(std::string pattern, ReadingStrategy* reading_strategy) {
    
    std::list<size_t> pointer_candidates(pointer_map[pattern].pointers.begin(), pointer_map[pattern].pointers.end());
    
    int count;
    int offset = 1;
    char most_frequent = '\0';

    while (most_frequent == '\0' || count > 1){

        count = 0;

        // Majority algorithm: first pass (determine most frequent)
        for (size_t pointer : pointer_candidates) {
            char char_at_pointer = reading_strategy->at(pointer + offset);

            if (count == 0) {
                most_frequent = char_at_pointer;
                count++;
            } else if (most_frequent == char_at_pointer) {
                count++;
            } else {
                count--;
            }
        }

        // Majority algorithm: second pass (remove all pointers that don't match the most frequent)
        for (std::list<size_t>::iterator it = pointer_candidates.begin(); it != pointer_candidates.end();) {
            size_t pointer = *it;
            if (reading_strategy->at(pointer + offset) == most_frequent)
                it++;
            else
                it = pointer_candidates.erase(it);
        }

        offset++;
    }

    size_t pointer_candidate = pointer_candidates.front();
    int i;
    for (i = 0; i < pointer_map[pattern].pointers.size(); i++)
        if (pointer_map[pattern].pointers[i] == pointer_candidate)
            break;
    pointer_map[pattern].copy_pointer_index = i;

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

    current_pattern = std::string(k, max_pair->first);
    copy_pattern = current_pattern;
}

// Return true if the pattern is already in the map
bool CopyModel::registerPattern() {
    return pointer_manager->registerCopyPointer(current_pattern, current_position);
}

void CopyModel::advance() {
    // Update current pattern and advance read pointer (current_position)
    current_pattern += reading_strategy->at(++current_position);
    current_pattern.erase(0, 1);
    // Advance copy pointer
    copy_position++;
    
}

// Returns true when able to predict
bool CopyModel::predictionSetup(bool pattern_has_past) {

    hit_probability = calculateProbability(
        pointer_manager->getHits(current_pattern),
        pointer_manager->getMisses(current_pattern));

    bool can_predict = copy_position != current_position;

    if (!can_predict && pattern_has_past) {
        // The copy_pointer_index is initialized at 0. Therefore, the copy_position should be the only other pointer that doesn't point to the current position
        copy_position = pointer_manager->getCopyPointer(current_pattern);
        copy_pattern = current_pattern;
    }
    // Check whether copy pointer should be changed
    else if (pointer_threshold->surpassedThreshold(hit_probability)) {

        pointer_manager->repositionCopyPointer(copy_pattern, reading_strategy);
        // Change copy pointer to a new one, this one being from the current pattern
        copy_pattern = current_pattern;
        copy_position = pointer_manager->getCopyPointer(current_pattern);

        pointer_manager->reset();
        pointer_threshold->reset();
    }

    return copy_position != current_position;
}

bool CopyModel::predict() {

    prediction = reading_strategy->at(copy_position + 1);
    actual = reading_strategy->at(current_position + 1);

    bool hit = prediction == actual;

    pointer_manager->reportPrediction(current_pattern, hit);

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

double CopyModel::calculateProbability(int hits, int misses) {
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

double CopyModel::progress() {
    return (double) current_position / reading_strategy->endOfStream();
}

void CopyModel::guess() {

    actual = reading_strategy->at(current_position + 1);
    
    // Just return the base distribution
    prediction = '\0';
    hit_probability = 0;
    // TODO: maybe we shouldn't copy like this! this allows for editing the base distribution from outside
    probability_distribution = base_distribution->distribution;
}
