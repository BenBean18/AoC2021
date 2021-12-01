#include <iostream>
#include <fstream>
#include <vector>

int main(int argc, char** argv) {
    std::string line;

    std::ifstream input_file("day1.txt");
    if (!input_file.is_open()) {
        std::cerr << "Error opening file" << std::endl;
        return EXIT_FAILURE;
    }

    int previousMeasurement = -1;
    int increased = 0;
    while (getline(input_file, line)) {
        int measurement = std::stoi(line);
        if (previousMeasurement != -1) {
            increased += (int)(measurement > previousMeasurement);
        }
        previousMeasurement = measurement;
    }
    input_file.close();

    std::cout << increased << std::endl;
    return 0;
}