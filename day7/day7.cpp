#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

std::vector<int> getInts() {
    std::vector<int> ints;
    std::string line;

    std::ifstream input_file("day7.txt");
    if (!input_file.is_open()) {
        std::cerr << "Error opening file" << std::endl;
        return {};
    }

    while (getline(input_file, line, ',')) {
        ints.push_back(std::stoi(line));
    }

    input_file.close();
    return ints;
}

int main(int argc, char** argv) {
    // median???
    // oh my gosh, it actually was. yay :)
    std::vector<int> ints = getInts();

    std::sort(ints.begin(), ints.end());
    // std::cout << ints.size() << std::endl; // =1000
    int median = (ints[ints.size()/2] + ints[(ints.size()/2) - 1])/2; // size is even
    std::cout << median << std::endl;
    // unsigned long minDistance = std::numeric_limits<unsigned long>::max();
    unsigned long distance = 0;
    for (int i : ints) {
        distance += abs(i - median);
    }
    std::cout << distance << std::endl;

    return 0;
}