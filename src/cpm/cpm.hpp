#include <vector>
#include <string>
#include <map>
#include "parser.hpp"


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


struct SimplePointerInfo {
    std::vector<size_t> pointers;
    int copy_pointer_index;     // Pattern's last symbol
};

class CopyPointerManager {
public:
    virtual int getCopyPointer(std::string) = 0;
    virtual void repositionCopyPointer(std::string, ReadingStrategy*) = 0;
    virtual bool registerCopyPointer(std::string, size_t) = 0;
    virtual void reportPrediction(std::string, bool) = 0;
    virtual void reset() = 0;
    virtual int getHits(std::string) = 0;
    virtual int getMisses(std::string) = 0;
};

class SimpleCopyPointerManager : public CopyPointerManager {

protected:
    std::map<std::string, SimplePointerInfo> pointer_map;
    int hits = 0;
    int misses = 0;

public:
    int getCopyPointer(std::string);
    bool registerCopyPointer(std::string, size_t);
    void reportPrediction(std::string, bool);
    void reset();
    int getHits(std::string);
    int getMisses(std::string);
};

class MostCommonCopyPointerManager : public SimpleCopyPointerManager {
public:
    void repositionCopyPointer(std::string, ReadingStrategy*);    
};

class RecentCopyPointerManager : public SimpleCopyPointerManager {
public:
    void repositionCopyPointer(std::string, ReadingStrategy*);
};

class NextOldestCopyPointerManager : public SimpleCopyPointerManager {
public:
    void repositionCopyPointer(std::string, ReadingStrategy*);
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

    friend MostCommonCopyPointerManager;

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
    double calculateProbability(int, int);
    void setRemainderProbabilities(char, double);

};