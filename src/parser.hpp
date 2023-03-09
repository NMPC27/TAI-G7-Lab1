#include <vector>

class ReadingStrategy {

public:
    virtual void read(char);
    virtual char at(int);
    virtual size_t end_of_stream();

};

class InMemoryReadingStrategy : public ReadingStrategy {

    std::vector<char> mem_file;

public:
    InMemoryReadingStrategy();
    void read(char);
    char at(size_t);
    size_t end_of_stream();
    void clean();

};
