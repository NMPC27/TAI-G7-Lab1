#include "parser.hpp"

void InMemoryReadingStrategy::read(char c) {
    this->mem_file.push_back(c);
}

char InMemoryReadingStrategy::at(size_t pos) {
    return this->mem_file[pos];
}

size_t InMemoryReadingStrategy::end_of_stream() {
    return this->mem_file.size();
}

void InMemoryReadingStrategy::appendPrediction(char predicted) {
    this->mem_file.push_back(predicted);
}

void InMemoryReadingStrategy::clean() {
    this->mem_file.clear();
}