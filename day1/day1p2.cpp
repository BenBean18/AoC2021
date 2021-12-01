#include <iostream>
#include <fstream>
#include <vector>

int sumOfWindow(std::vector<int> inputs, size_t startingIndex) {
    return inputs[startingIndex] + inputs[startingIndex+1] + inputs[startingIndex+2];
}

int main(int argc, char** argv) {
    std::string line;

    std::ifstream input_file("day1.txt");
    if (!input_file.is_open()) {
        std::cerr << "Error opening file" << std::endl;
        return EXIT_FAILURE;
    }

    int previousWindow = -1;
    int increased = 0;
    std::vector<int> inputs;
    while (getline(input_file, line)) {
        int measurement = std::stoi(line);
        inputs.push_back(measurement);
    }
    input_file.close();

    for (size_t i = 0; i < inputs.size() - 2; i++) {
        int window = sumOfWindow(inputs, i);
        if (previousWindow != -1) {
            increased += (int)(window > previousWindow);
        }
        previousWindow = window;
    }
    

    std::cout << increased << std::endl;
    return 0;
}