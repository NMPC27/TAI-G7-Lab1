#include <vector>
#include <string>
#include <map>
#include "parser.hpp"


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