#include <iostream>
#include <fstream>
#include <vector>
#include <numeric>
#include <math.h>
#include <sstream>

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

int main(int argc, char** argv) {
    // Insert code here! Use snippets -- "strs" = strings, "ints" = ints, "oth" = other
    // std::vector<int> ints = getInts(2); // base 2, it's binary
    // long long sum = std::accumulate(ints.begin(), ints.end(), 0);
    // std::cout << sum << std::endl;
    // // 12 bits, 2**11 is the first bit
    // std::cout << (sum / pow(2, 11)) / ints.size() << std::endl;

    std::vector<std::string> strings = getStrings();
    std::vector<float> counts;
    for (char i : strings[0]) {
        counts.push_back(0);
    }
    for (std::string i : strings) {
        for (int j = 0; j < i.size(); j++) {
            counts[j] += i[j] - '0';
        }
    }
    int num = 0;
    std::string gammaStr;
    for (int i = 0; i < counts.size(); i++) {
        counts[i] /= (float)(strings.size());
        counts[i] = round(counts[i]);
        gammaStr += '0' + counts[i];
    }
    int gamma = std::stoi(gammaStr, nullptr, 2); // base 2, binary
    int epsilon = pow(2, gammaStr.size()) - 1 - gamma; // 2**size - 1 is the maximum value in a binary number with the length size.
    // Epsilon is inverted gamma, so maximum - gamma = epsilon
    std::cout << epsilon * gamma << std::endl;

    return 0;
}