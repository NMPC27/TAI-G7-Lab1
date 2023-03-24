#include <vector>
#include <string>
#include <map>


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