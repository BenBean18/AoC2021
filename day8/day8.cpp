#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <regex>

std::vector<std::string> getStrings() {
    std::vector<std::string> strings;
    std::string line;

    std::ifstream input_file("day8.txt");
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

std::vector<int> getInts() {
    std::vector<int> ints;
    std::string line;

    std::ifstream input_file("day8.txt");
    if (!input_file.is_open()) {
        std::cerr << "Error opening file" << std::endl;
        return {};
    }

    while (getline(input_file, line)) {
        ints.push_back(std::stoi(line));
    }

    input_file.close();
    return ints;
}

std::vector<std::pair<std::array<std::string, 10>, std::array<std::string, 4>>> parseInput() {
    std::vector<std::string> strings = getStrings();
    std::regex sevenSegmentRegex("([^\\s]+) ([^\\s]+) ([^\\s]+) ([^\\s]+) ([^\\s]+) ([^\\s]+) ([^\\s]+) ([^\\s]+) ([^\\s]+) ([^\\s]+) \\| ([^\\s]+) ([^\\s]+) ([^\\s]+) ([^\\s]+)");
    std::vector<std::pair<std::array<std::string, 10>, std::array<std::string, 4>>> displays;

    for (std::string s : strings) {
        std::pair<std::array<std::string, 10>, std::array<std::string, 4>> line;
        std::smatch match;
        std::regex_match(s, match, sevenSegmentRegex);
        if (match.size() == 15) {   
            for (int i = 0; i < 10; i++) {
                line.first[i] = match[i+1].str();
                std::cout << line.first[i] << " ";
            }
            std::cout << "| ";
            for (int i = 0; i < 4; i++) {
                line.second[i] = match[i+11].str();
                std::cout << line.second[i] << " ";
            }
            displays.push_back(line);
            std::cout << std::endl;
        }
    }
    return displays;
}

int main(int argc, char** argv) {
    // Insert code here! Use snippets -- "strs" = strings, "ints" = ints, "oth" = other
    auto displays = parseInput();
    int count = 0;
    for (auto display : displays) {
        for (std::string s : display.second) {
            if (s.size() == 2 || s.size() == 3 || s.size() == 4 || s.size() == 7) {
                count++;
            }
        }
    }
    std::cout << count << std::endl;
    return 0;
}