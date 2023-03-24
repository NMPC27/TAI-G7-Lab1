#include <vector>
#include <string>
#include <map>


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