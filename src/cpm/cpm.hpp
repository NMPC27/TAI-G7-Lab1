#include <vector>
#include <string>
#include <map>
#include "parser.hpp"
#include "copy_pointer_threshold.hpp"
#include "copy_pointer_manager.hpp"
#include "base_distribution.hpp"


class CopyModel {

    int k;
    double alpha;
    ReadingStrategy* reading_strategy;
    CopyPointerThreshold* pointer_threshold;
    CopyPointerManager* pointer_manager;
    BaseDistribution* base_distribution;

    std::map<char, int> alphabet_counts;
    
    size_t current_position = -1;
    std::string current_pattern;
    size_t copy_position = -1;
    std::string copy_pattern;

public:
    CopyModel(int k, double alpha, ReadingStrategy* rs, CopyPointerThreshold* pt, CopyPointerManager* pm, BaseDistribution* bd) : k(k), alpha(alpha), reading_strategy(rs), pointer_threshold(pt), pointer_manager(pm), base_distribution(bd) {}
    bool registerPattern();
    bool predictionSetup(bool);
    bool predict();
    void advance();
    void firstPass(std::string);
    bool eof();
    int countOf(char);
    void guess();
    double progress();
    
    void initializeWithMostFrequent();

    // Read-only values. Always overwritten when calling predictNext()
    std::map<char, double> probability_distribution;
    double hit_probability = 0;
    char prediction = '\0';
    char actual = '\0';

private:
    double calculateProbability(int, int);
    void setRemainderProbabilities(char, double);

};