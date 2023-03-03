#include <iostream>
#include <fstream>
#include <map>
#include <algorithm>
#include <string>
#include <list>
#include <vector>
#include <cmath>

using namespace std;

void firstPass(string fileName);
void cpm(int k, float alpha);
double calculateProbability(int hits, int misses, float alpha);
void printMap(map<string, list<int>> map);
void printAlphabet(map<char, int> map);

vector<int> arr_file;
map<char, int> alphabet;

int main(int argc, char** argv) {

    if (argc != 3) {
        cout << "Usage: " << argv[0] << "<k> <file>" << endl;
        return 1;
    }


    int k = stoi(argv[1]);
    string fileName = argv[2];

    firstPass(fileName);
    
    float alpha = 1.0;

    cpm(k, alpha);

    return 0;
}

void firstPass(string fileName) {

    ifstream file(fileName);

    char c = file.get();
    
    while (!file.eof()) {

        arr_file.push_back((int) c);

        alphabet.insert({c, 0});
        alphabet[c]++;

        c = file.get();
    }

    file.close();
}

struct PatternInfo {
    vector<size_t> pointers;
    int copy_pointer_index;     // Pattern's last symbol
    int hits;
    int misses;
};

void cpm(int k, float alpha) {

    char c;

    map<string, PatternInfo> pointer_map; // map dos padroes, lista das posições onde forem encontradas

    string pattern = "";

    for (int i = 0; i < k-1; i++)
    {
        pattern += arr_file[i];
    }
    
    double information_sum;

    for (size_t pos = k-1; pos < arr_file.size(); pos++)
    {
        c = arr_file[pos];//le o caracter

        pattern += c; // adiciona o caracter ao padrao

        if (pointer_map.count(pattern) == 0) { //padrao nao esta no map

            struct PatternInfo pattern_info = {
                {pos}, 0, 0, 0
            };

            pointer_map.insert({pattern, pattern_info}); // insere o padrao no map com a posição atual

        } else { //padrao ja esta no map

            pointer_map[pattern].pointers.push_back(pos);// adiciona a posição atual ao padrao

            // Predict
            int predict_index = pointer_map[pattern].pointers[pointer_map[pattern].copy_pointer_index] + 1;

            // caracteres: previsão e o verdadeiro
            int prediction = arr_file[predict_index];
            int actual = arr_file[pos + 1];

            if (prediction == actual) {
                pointer_map[pattern].hits++;

                printf("Hit: %s, Prediction: %c\n", pattern.c_str(), prediction);
            } else {
                pointer_map[pattern].misses++;

                printf("Miss: %s, Prediction: %c, Actual: %c\n", pattern.c_str(), prediction, actual);
            }

            double hit_probability = calculateProbability(pointer_map[pattern].hits, pointer_map[pattern].misses, alpha);
            
            //! Calculate information of current character  ???
            information_sum += -log2((prediction == actual) ? hit_probability : ((1 - hit_probability) / (alphabet.size() - 1)));

            // Check whether copy pointer should be changed
            if (hit_probability < 0.5) {
                pointer_map[pattern].copy_pointer_index++;

                pointer_map[pattern].hits = 0;
                pointer_map[pattern].misses = 0;

                printf("Hit probability below threshold, choosing copy pointer %d\n", pointer_map[pattern].copy_pointer_index);
            }

        }


        pattern.erase(0, 1); // remove o primeiro caracter do padrao

    }

    cout << "Amount of information: " << information_sum << endl;

    //printMap(map);
    printAlphabet(alphabet);
}

double calculateProbability(int hits, int misses, float alpha) {
    return (hits + alpha) / (hits + misses + 2 * alpha);
}

void printAlphabet(map<char, int> pointer_map) {

    for (auto it = pointer_map.begin(); it != pointer_map.end(); it++) {
        cout << it->first << " : " << it->second << endl;
    }
}

void printMap(map<string, list<int>> pointer_map) {

    for (auto it = pointer_map.begin(); it != pointer_map.end(); it++) {
        cout << it->first << " : ";
        for (auto it2 = it->second.begin(); it2 != it->second.end(); it2++) {
            cout << *it2 << " ";
        }
        cout << endl;
    }
}