#include <vector>
#include <string>
#include <list>
#include <map>


void outputProbabilityDistribution(char prediction, char actual, double hit_probability, std::map<char, double> base_distribution);

// Options
void printUsage(char* prog_name);
void printOptions();

// Helper functions
void printAlphabet(std::map<char, double> map);
void printMap(std::map<std::string, std::list<int>> map);
