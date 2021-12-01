#include <iostream>
#include <fstream>
#include <vector>

int main(int argc, char** argv) {
    std::vector<std::string> lines;
    std::string line;

    std::ifstream input_file("day1.txt");
    if (!input_file.is_open()) {
        std::cerr << "Error opening file" << std::endl;
        return EXIT_FAILURE;
    }

    while (getline(input_file, line)) {
        lines.push_back(line);
        // or do something else
    }

    input_file.close();
    return 0;
}