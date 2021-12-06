#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <numeric>

std::vector<std::string> getStrings(char delim = '\n') {
    std::vector<std::string> strings;
    std::string line;

    std::ifstream input_file("day6.txt");
    if (!input_file.is_open()) {
        std::cerr << "Error opening file" << std::endl;
        return {};
    }

    while (getline(input_file, line, delim)) {
        strings.push_back(line);
    }

    input_file.close();
    return strings;
}

std::vector<int> getInts(char delim = '\n') {
    std::vector<int> ints;
    std::string line;

    std::ifstream input_file("day6.txt");
    if (!input_file.is_open()) {
        std::cerr << "Error opening file" << std::endl;
        return {};
    }

    while (getline(input_file, line, delim)) {
        ints.push_back(std::stoi(line));
    }

    input_file.close();
    return ints;
}

void updateCounts(std::array<long long, 9> &counts) {
    std::array<long long, 9> countsCopy;
    for (size_t i = 0; i < 9; i++) {
        countsCopy[i] = counts[i];
    }
    for (size_t i = 8; i > 0; i--) {
        counts[i-1] = countsCopy[i];
    }
    // 0 -> 6
    counts[0] = countsCopy[1];
    counts[6] += countsCopy[0];
    // and add 8
    counts[8] = countsCopy[0];
}

int main(int argc, char** argv) {
    std::vector<int> ints = getInts(',');
    long long days = 256;
    long long fish = 0;
    std::array<long long, 9> counts; // NOTE do not look at the reddit solutions page in the future, it makes it so you can't figure it out by yourself :'(
    counts.fill(0);
    for (long long i : ints) {
        counts[i] += 1;
    }
    for (int i = 0; i < days; i++) {
        updateCounts(counts);
    }
    for (long long i : counts) {
        std::cout << i << "+";
    }
    return 0;
}