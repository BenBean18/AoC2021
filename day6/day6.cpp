#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

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

double exponentialGrowth(double initial, double rate, double intervals) {
    return powf64((initial * (1 + rate)), intervals);
}

int simulateLanternfish(int internalTimer, int days, int start = 0) {
    int lanternfish = 1;
    for (int i = start; i < days; i++) {
        if (internalTimer == 0) {
            internalTimer = 6;
            lanternfish += simulateLanternfish(8, days, i+1);
        } else if (internalTimer > 0) {
            internalTimer--;
        }
    }
    return lanternfish;
}

int main(int argc, char** argv) {
    std::vector<int> ints = getInts(',');
    int days = 80;
    // int fish = simulateLanternfish(3, days) + simulateLanternfish(4, days) + simulateLanternfish(3, days) + simulateLanternfish(1, days) + simulateLanternfish(2, days);
    int fish = 0;
    for (int i : ints) {
        fish += simulateLanternfish(i, days);
    }
    std::cout << fish << std::endl;
    return 0;
}