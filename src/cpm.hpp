#include <vector>
#include <string>
#include <list>
#include <map>


void firstPass(std::string fileName);
void cpm(int k, double alpha);
double calculateProbability(int hits, int misses, double alpha);

// Options
void printUsage(char* prog_name);
void printOptions();

// Helper functions
void printAlphabet(std::map<char, double> map);
void printMap(std::map<std::string, std::list<int>> map);
