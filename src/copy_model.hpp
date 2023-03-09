#include <vector>
#include <string>
#include <map>
#include "parser.hpp"

struct PatternInfo {
    std::vector<size_t> pointers;
    int copy_pointer_index;     // Pattern's last symbol
    int hits;
    int misses;
};

class CopyPointerThreshold {
public:
    virtual bool surpassedThreshold(double hit_probability);
};

class StaticCopyPointerThreshold : public CopyPointerThreshold {
public:
    bool surpassedThreshold(double hit_probability);
};

class CopyPointerManager {
public:
    virtual int newCopyPointer(std::vector<size_t> copy_pointers, int current_copy_pointer);
};

class RecentCopyPointerManager : public CopyPointerManager {
public:
    int newCopyPointer(std::vector<size_t> copy_pointers, int current_copy_pointer);
};

class BaseDistribution {
public:
    virtual void setBaseDistribution(std::map<char, int> histogram);
    std::map<char, double> distribution;
};

class UniformDistribution : public BaseDistribution {
public:
    void setBaseDistribution(std::map<char, int> histogram);
};

class FrequencyDistribution : public BaseDistribution {
public:
    void setBaseDistribution(std::map<char, int> histogram);
};

class CopyModel {

    int k;
    double alpha;
    ReadingStrategy* reading_strategy;
    CopyPointerThreshold* pointer_threshold;
    CopyPointerManager* pointer_manager;
    BaseDistribution* base_distribution;

    std::map<char, int> alphabet_counts;
    std::map<std::string, PatternInfo> pointer_map;
    
    size_t current_position;
    std::string current_pattern;

public:
    CopyModel(int k, double alpha, ReadingStrategy* rs, CopyPointerThreshold* pt, CopyPointerManager* pm, BaseDistribution* bd) : k(k), alpha(alpha), reading_strategy(rs), pointer_threshold(pt), pointer_manager(pm), base_distribution(bd) {}
    bool predict();
    bool advance();
    void firstPass(std::string);
    bool eof();
    void reset();
    
    void initializeWithMostFrequent();

    // Read-only values. Always overwritten when calling predictNext()
    std::map<char, double> probability_distribution;
    double hit_probability;
    char prediction;

private:
    double calculateProbability();
    void setRemainderProbabilities(char, double);

};