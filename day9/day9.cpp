#include <iostream>
#include <fstream>
#include <vector>

std::vector<std::string> getStrings() {
    std::vector<std::string> strings;
    std::string line;

    std::ifstream input_file("day9.txt");
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

std::vector<std::vector<int>> parseData() {
    std::vector<std::vector<int>> ints;
    std::string line;

    std::ifstream input_file("day9.txt");
    if (!input_file.is_open()) {
        std::cerr << "Error opening file" << std::endl;
        return {};
    }

    while (getline(input_file, line)) {
        std::vector<int> l;
        for (char c : line) {
            l.push_back(c - '0');
        }
        ints.push_back(l);
    }

    input_file.close();
    return ints;
}

std::vector<int> adjacentNumbers(std::vector<std::vector<int>> data, std::pair<int, int> coordinates) {
    std::vector<std::pair<int, int>> adjacentCoords = {{coordinates.first-1,coordinates.second},{coordinates.first+1,coordinates.second},{coordinates.first,coordinates.second-1},{coordinates.first,coordinates.second+1}};
    std::vector<int> adjacent;
    for (auto coord : adjacentCoords) {
        if (!(coord.first < 0 || coord.first >= data[0].size() || coord.second < 0 || coord.second >= data.size())) {
            adjacent.push_back(data[coord.second][coord.first]);
        }
    }
    return adjacent;
}

// Coordinates are x,y and the list is indexed [y][x]
bool lowerThanAdjacent(std::vector<std::vector<int>> data, std::pair<int, int> coordinates) {
    if (data[coordinates.second][coordinates.first] == 0) { // no zeroes in a row
        return true;
    }
    if (data[coordinates.second][coordinates.first] == 9) { // 9 is the highest
        return false;
    }
    bool lower = true;
    for (int i : adjacentNumbers(data, coordinates)) {
        if (data[coordinates.second][coordinates.first] >= i) {
            lower = false;
        }
    }
    return lower;
}

int main(int argc, char** argv) {
    auto data = parseData();
    unsigned long riskLevelSum = 0;
    for (int y = 0; y < data.size(); y++) {
        for (int x = 0; x < data[0].size(); x++) {
            if (lowerThanAdjacent(data, {x, y})) {
                riskLevelSum += data[y][x] + 1;
            }
        }
    }
    std::cout << riskLevelSum << std::endl;
    return 0;
}