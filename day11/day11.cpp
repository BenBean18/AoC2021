#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <thread>
#include <chrono>

std::vector<std::string> getStrings() {
    std::vector<std::string> strings;
    std::string line;

    std::ifstream input_file("input.txt");
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

    std::ifstream input_file("input.txt");
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

std::vector<std::pair<int, int>> adjacentCoords(std::pair<int, int> c) {
	int x, y;
	x = c.first;
	y = c.second;
	return {{x-1,y-1},{x-1,y},{x-1,y+1},{x,y-1},/*skipping the value itself*/{x,y+1},{x+1,y-1},{x+1,y},{x+1,y+1}};
}

std::vector<std::vector<int>> parseInput() {
    std::vector<std::string> strings = getStrings();
    std::vector<std::vector<int>> parsed;
    for (std::string s : strings) {
        std::vector<int> line;
        for (char c : s) {
            line.push_back(c - '0');
        }
        parsed.push_back(line);
    }
    return parsed;
}

int main(int argc, char** argv) {
    // 2D vector of puzzle input numbers.
    std::vector<std::vector<int>> data = parseInput();
    // Also, a 2D vector of which have already flashed.
    std::vector<std::vector<bool>> alreadyFlashed(data.size());
    // Flash count = 0
    unsigned long flashCount = 0;
    int steps = 100;
    double flashSpeed = 0; // 0 = off
    // For each step:
    for (int _ = 0; _ < steps; _++) {
        // Reset 2D vector of already flashed to false
        for (auto &v : alreadyFlashed) { // without this reference (&v), i was getting a segfault... :)
            v.resize(data[0].size());
            std::fill(v.begin(), v.end(), false);
        }
        // Increase all in 2D array by 1
        for (int y = 0; y < data.size(); y++) {
            for (int x = 0; x < data[0].size(); x++) {
                data[y][x] += 1;
            }
        }
        // While there are still items in the 2D array > 9:
        int above9 = std::count_if(data.begin(), data.end(), [](std::vector<int> i){return std::count_if(i.begin(), i.end(), [](int i){return i > 9;}) > 0;});
        while (above9 > 0) {
            std::cout << "\033[1H";
            for (int y = 0; y < data.size(); y++) {
                std::cout << "\r               \r";
                for (int x = 0; x < data[0].size(); x++) {
                    if (alreadyFlashed[y][x]) {
                        std::cout << "\033[38;5;226m";
                    }
                    std::cout << "*";
                    std::cout << "\033[0m";
                }
                std::cout << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds((int)(50 * flashSpeed)));
            // For each y in the 2D array:
            for (int y = 0; y < data.size(); y++) {
                // For each x in the 2D array:
                for (int x = 0; x < data[0].size(); x++) {
                    // If array[y][x] > 9 && alreadyFlashed[y][x] == false:
                    if ((data[y][x] > 9) && (alreadyFlashed[y][x] == false)) {
                        alreadyFlashed[y][x] = true;
                        flashCount++;
                        data[y][x] = 0;
                        // For each coordinate in adjacentCoords({x, y}):
                        for (auto c : adjacentCoords({x, y})) {
                            try {
                                if (!alreadyFlashed.at(c.second).at(c.first)) { // <- needed this
                                    data.at(c.second).at(c.first) += 1; // I accidentally had x and y instead of c.first and c.second... <facepalm>
                                    // And then I flipped c.second and c.first...
                                }
                            } catch (...) {
                                // out of bounds, do nothing
                            }
                        }
                    }
                }
            }
            above9 = std::count_if(data.begin(), data.end(), [](std::vector<int> i){return std::count_if(i.begin(), i.end(), [](int i){return i > 9;}) > 0;});
        }
        std::cout << "\033[1H";
        for (int y = 0; y < data.size(); y++) {
            std::cout << "\r               \r";
            for (int x = 0; x < data[0].size(); x++) {
                if (alreadyFlashed[y][x]) {
                    std::cout << "\033[38;5;226m";
                }
                std::cout << "*";
                std::cout << "\033[0m";
            }
            std::cout << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds((int)(100 * flashSpeed)));
        std::cout << flashCount << std::endl;
    }
    std::cout << flashCount << std::endl;
    return 0;
}