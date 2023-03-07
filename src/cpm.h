#include <vector>
#include <map>
#include <string>
#include <list>


void firstPass(std::string fileName);

void cpm(int k, double alpha);

double calculateProbability(int hits, int misses, double alpha);

void printMap(std::map<std::string, std::list<int>> map);

void printAlphabet(std::map<char, double> map);

void printUsage(char* prog_name);

void printOptions();

//void surpassedThreshold();
