#include <iostream>
#include <fstream>
#include <vector>
#include <numeric>
#include <math.h>
#include <sstream>
#include <algorithm>

// When removing unpopular items, subtract 1 (if 1) from the next index in counts

std::vector<std::string> getStrings() {
    std::vector<std::string> strings;
    std::string line;

    std::ifstream input_file("day3.txt");
    if (!input_file.is_open()) {
        std::cerr << "Error opening file" << std::endl;
        return {};
    }

    while (getline(input_file, line)) {
        strings.push_back(line);
    }

    input_file.close();
    return strings;
}

std::vector<int> getInts(int base = 10) {
    std::vector<int> ints;
    std::string line;

    std::ifstream input_file("day3.txt");
    if (!input_file.is_open()) {
        std::cerr << "Error opening file" << std::endl;
        return {};
    }

    while (getline(input_file, line)) {
        ints.push_back(std::stoi(line, nullptr, base));
    }

    input_file.close();
    return ints;
}

std::vector<int> mostCommon(std::vector<std::string> strings) {
    // do stuff
}

int main(int argc, char** argv) {
    // Insert code here! Use snippets -- "strs" = strings, "ints" = ints, "oth" = other

    std::vector<std::string> stringsO2 = getStrings();
    std::vector<std::string> stringsCO2 = getStrings();
    std::vector<float> countsO2;
    std::vector<float> countsCO2;
    std::string O2 = "";
    std::string CO2 = "";
    for (char i : stringsO2[0]) {
        countsO2.push_back(0);
        countsCO2.push_back(0);
    }
    for (std::string i : stringsO2) {
        for (int j = 0; j < i.size(); j++) {
            countsO2[j] += i[j] - '0';
            countsCO2[j] += i[j] - '0';
        }
    }
    // do O2
    for (int i = 0; i < stringsO2[0].size(); i++) { // iterate through each bit
        // copy vectors, will be deleting
        std::vector<std::string> stringsO2_copy = stringsO2;
        std::vector<float> countsO2_copy = countsO2;
        // if only one left, that's it -- break
        if (stringsO2.size() == 1) {
            break;
        }
        for (int s = 0; s < stringsO2_copy.size(); s++) {
            // need to round where 0.5 = 0 for CO2 and 0.5 = 1 for O2
            // if the digit is [unpopular or 0 if equal, for O2] or [1 - (see columns 33-64), for CO2]
            if (stringsO2_copy[s][i] != ((char)round(countsO2_copy[i]/(float)stringsO2_copy.size()) + '0')) {
                // subtract from counts (so we don't have to recompute, just make counts like it's not there)
                for (int j = i; j < countsO2.size(); j++) {
                    countsO2[j] -= (stringsO2_copy[s][j] - '0');
                }
                // remove from strings
                stringsO2.erase(std::remove(stringsO2.begin(), stringsO2.end(), stringsO2_copy[s]), stringsO2.end());
            }
        }
    }
    O2 = stringsO2[0];
    // do CO2
    for (int i = 0; i < stringsCO2[0].size(); i++) { // iterate through each bit
        // copy vectors, will be deleting
        std::vector<std::string> stringsCO2_copy = stringsCO2;
        std::vector<float> countsCO2_copy = countsCO2;
        // if only one left, that's it -- break
        if (stringsCO2.size() == 1) {
            break;
        }
        for (int s = 0; s < stringsCO2_copy.size(); s++) {
            // need to round where 0.5 = 0 for CO2 and 0.5 = 1 for O2
            // if the digit is [unpopular or 0 if equal, for O2] or [1 - (see columns 33-64), for CO2]
            if (stringsCO2_copy[s][i] != ((1 - (char)round(countsCO2_copy[i]/(float)stringsCO2_copy.size())) + '0')) {
                // subtract from counts (so we don't have to recompute, just make counts like it's not there)
                for (int j = i; j < countsCO2.size(); j++) {
                    countsCO2[j] -= (stringsCO2_copy[s][j] - '0');
                }
                // remove from strings
                stringsCO2.erase(std::remove(stringsCO2.begin(), stringsCO2.end(), stringsCO2_copy[s]), stringsCO2.end());
            }
        }
    }
    CO2 = stringsCO2[0];
    std::cout << "O2: " << O2 << ", CO2: " << CO2 << std::endl;
    std::cout << std::stoi(O2, nullptr, 2) * std::stoi(CO2, nullptr, 2) << std::endl;

    return 0;
}