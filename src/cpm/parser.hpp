#include <vector>
#ifndef PARSER_HPP
#define PARSER_HPP

class ReadingStrategy {

public:
    virtual ~ReadingStrategy() {};
    virtual void read(char) = 0;
    virtual char at(size_t) = 0;
    virtual size_t endOfStream() = 0;

};

class InMemoryReadingStrategy : public ReadingStrategy {

    std::vector<char> mem_file;

public:
    void read(char);
    char at(size_t);
    size_t endOfStream();
    void clean();

};

#endif