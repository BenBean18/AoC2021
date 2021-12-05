#include <iostream>
#include <fstream>
#include <vector>
#include <regex>

std::vector<std::string> getStrings() {
    std::vector<std::string> strings;
    std::string line;

    std::ifstream input_file("day5.txt");
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

std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> getLines(bool horizontalOnly = true) {
    std::regex hydroVentRegex("(\\d+),(\\d+) -> (\\d+),(\\d+)");
    std::vector<std::string> strings = getStrings();
    std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> lines{}; // a pair of pairs...
    for (std::string s : strings) {
        std::pair<std::pair<int, int>, std::pair<int, int>> line;
        std::smatch match;
        std::regex_match(s, match, hydroVentRegex);
        if (match.size() == 5) {
            line.first.first = std::stoi(match[1].str()); // x1
            line.first.second = std::stoi(match[2].str()); // y1
            line.second.first = std::stoi(match[3].str()); // x2
            line.second.second = std::stoi(match[4].str()); // y2
        } else {
            std::cerr << "Less than 5 matches found" << std::endl;
        }
        // std::cout << (s == (std::to_string(line.first.first) + "," + std::to_string(line.first.second) + " -> " + std::to_string(line.second.first) + "," + std::to_string(line.second.second))) << std::endl;
        lines.push_back(line);
    }
    if (horizontalOnly) {
        auto linesCopy(lines);
        for (auto line : linesCopy) {
            if (!((line.first.first == line.second.first) || (line.first.second == line.second.second))) {
                lines.erase(std::remove(lines.begin(), lines.end(), line), lines.end());
            }
        }
    }
    return lines;
}

int main(int argc, char** argv) {
    auto lines = getLines();
    std::array<std::array<int, 1000>, 1000> diagram; // diagram[y][x]
    // fill diagram with zeros
    std::array<int, 1000> a;
    a.fill(0);
    diagram.fill(a);
    // for (auto a : diagram) { // fill diagram with zeros
    //     a.fill(0);
    // }
    for (auto line : lines) {
        int xMin = std::min(line.first.first, line.second.first);
        int xMax = std::max(line.first.first, line.second.first);
        int yMin = std::min(line.first.second, line.second.second);
        int yMax = std::max(line.first.second, line.second.second);
        bool xEqual = xMin == xMax;
        bool yEqual = yMin == yMax;
        if (xEqual) {
            for (int y = yMin; y <= yMax; y++) {
                diagram[y][xMin] += 1;
            }
        }
        if (yEqual) {
            for (int x = xMin; x <= xMax; x++) {
                diagram[yMin][x] += 1;
            }
        }
    }
    int moreThan2Overlap = 0;
    for (auto a : diagram) {
        for (int i = 0; i < a.size(); i++) {
            moreThan2Overlap += (a[i] >= 2);
        }
    }
    std::cout << moreThan2Overlap << std::endl;
    return 0;
}