#include <vector>

class ReadingStrategy {

public:
    virtual void read(char) = 0;
    virtual char at(size_t) = 0;
    virtual size_t end_of_stream() = 0;

};

class InMemoryReadingStrategy : public ReadingStrategy {

    std::vector<char> mem_file;

public:
    void read(char);
    char at(size_t);
    size_t end_of_stream();
    void clean();

};
