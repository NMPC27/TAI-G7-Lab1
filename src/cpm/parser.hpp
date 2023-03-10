#include <vector>

class ReadingStrategy {

public:
    virtual void read(char) = 0;
    virtual char at(size_t) = 0;
    virtual size_t endOfStream() = 0;
    virtual void appendPrediction(char) = 0;

};

class InMemoryReadingStrategy : public ReadingStrategy {

    std::vector<char> mem_file;

public:
    void read(char);
    char at(size_t);
    size_t endOfStream();
    void appendPrediction(char);
    void clean();

};
