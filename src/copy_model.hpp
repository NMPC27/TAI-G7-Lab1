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

class CopyModel {

    double alpha;
    ReadingStrategy* reading_strategy;
    CopyPointerThreshold* pointer_threshold;
    CopyPointerManager* pointer_manager;

    std::map<char, int> alphabet_counts;
    std::map<std::string, PatternInfo> pointer_map;
    
    int current_position;
    std::string current_pattern;

public:
    CopyModel(double alpha, ReadingStrategy* rs, CopyPointerThreshold* pt, CopyPointerManager* pm) : alpha(alpha), reading_strategy(rs), pointer_threshold(pt), pointer_manager(pm) {}
    bool predictNext();
    void firstPass(std::string);
    bool eof();
    void reset();

    // Read-only values. Always overwritten when calling predictNext()
    std::map<char, double> probability_distribution;
    double hit_probability;
    char prediction;

private:
    double calculateProbability();
    void setRemainderProbabilities(char, double);

};