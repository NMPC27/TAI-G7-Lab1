#include <vector>
#include <string>
#include <map>
#include "parser.hpp"
#include "cpm.hpp"


class CopyModelOld : public CopyModel {

    int k;
    double alpha;
    ReadingStrategy* reading_strategy;
    CopyPointerThreshold* pointer_threshold;
    CopyPointerManager* pointer_manager;
    BaseDistribution* base_distribution;

    std::map<char, int> alphabet_counts;
    std::map<std::string, PatternInfo> pointer_map;
    
    size_t current_position = 0;
    std::string current_pattern;

public:
    CopyModelOld(int k, double alpha, ReadingStrategy* rs, CopyPointerThreshold* pt, CopyPointerManager* pm, BaseDistribution* bd) : k(k), alpha(alpha), reading_strategy(rs), pointer_threshold(pt), pointer_manager(pm), base_distribution(bd) {}
    void registerPattern();
    bool predict();
    void advance();
    void firstPass(std::string);
    bool eof();
    void reset();
    
    void initializeWithMostFrequent();

    // Read-only values. Always overwritten when calling predictNext()
    std::map<char, double> probability_distribution;
    double hit_probability = 0;
    char prediction = '\0';

private:
    double calculateProbability();
    void setRemainderProbabilities(char, double);

};