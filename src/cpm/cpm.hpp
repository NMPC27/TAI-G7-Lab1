#include <vector>
#include <string>
#include <map>
#include "parser.hpp"

struct PatternInfo {
    std::vector<size_t> pointers;
    int copy_pointer_index;     // Pattern's last symbol
};

class CopyPointerThreshold {
public:
    virtual bool surpassedThreshold(double hit_probability) = 0;
    virtual void reset() = 0;
};

class StaticCopyPointerThreshold : public CopyPointerThreshold {

    double static_threshold;

public:
    StaticCopyPointerThreshold(double st) : static_threshold(st) {}
    bool surpassedThreshold(double hit_probability);
    void reset();
};

class DerivativeCopyPointerThreshold : public CopyPointerThreshold {

    double previous_hit_probability = -1;
    double derivative_threshold;

public:
    DerivativeCopyPointerThreshold(double dt) : derivative_threshold(dt) {}
    bool surpassedThreshold(double hit_probability);
    void reset();
};

class SuccessFailsCopyPointerThreshold : public CopyPointerThreshold {

    double previous_hit_probability = -1;
    int fails_count = 0;
    int fails_threshold;

public:
    SuccessFailsCopyPointerThreshold(double dt) : fails_threshold(dt) {}
    bool surpassedThreshold(double hit_probability);
    void reset();
};

class CopyPointerManager {
public:
    virtual int newCopyPointer(std::vector<size_t> copy_pointers, int current_copy_pointer) = 0;
};

class RecentCopyPointerManager : public CopyPointerManager {
public:
    int newCopyPointer(std::vector<size_t> copy_pointers, int current_copy_pointer);
};

class NextOldestCopyPointerManager : public CopyPointerManager {
public:
    int newCopyPointer(std::vector<size_t> copy_pointers, int current_copy_pointer);
};

class BaseDistribution {
public:
    virtual void setBaseDistribution(std::map<char, int> histogram) = 0;
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
    
    size_t current_position = 0;
    std::string current_pattern;
    size_t copy_position = -1;
    std::string copy_pattern;
    int hits = 0;
    int misses = 0;
    bool first_prediction = true;

public:
    CopyModel(int k, double alpha, ReadingStrategy* rs, CopyPointerThreshold* pt, CopyPointerManager* pm, BaseDistribution* bd) : k(k), alpha(alpha), reading_strategy(rs), pointer_threshold(pt), pointer_manager(pm), base_distribution(bd) {}
    bool registerPattern();
    bool predict();
    void advance();
    void firstPass(std::string);
    bool eof();
    int countOf(char);
    void guess();
    
    void initializeWithMostFrequent();

    // Read-only values. Always overwritten when calling predictNext()
    std::map<char, double> probability_distribution;
    double hit_probability = 0;
    char prediction = '\0';
    char actual = '\0';

private:
    double calculateProbability();
    void setRemainderProbabilities(char, double);

};