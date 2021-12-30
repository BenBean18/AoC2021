#include <iostream>
#include <fstream>
#include <vector>
#include <chrono> // for sleeping
#include <thread>
#include <string.h>

void delay(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

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

char at(const std::vector<std::string> &data, size_t x, size_t y) { // account for wraparound on *either x or y, not both*
    if (x >= data[0].size()) {
        return data[y][x-data[0].size()];
    } else if (y >= data.size()) {
        return data[y-data.size()][x];
    } else {
        return data[y][x];
    }
}

void set(std::vector<std::string> &data, size_t x, size_t y, char c) { // account for wraparound on *either x or y, not both*
    if (x >= data[0].size()) {
        data[y][x-data[0].size()] = c;
    } else if (y >= data.size()) {
        data[y-data.size()][x] = c;
    } else {
        data[y][x] = c;
    }
}

void east(std::vector<std::string> &data) {
    std::vector<std::string> dataCopy = data;
    for (int y = 0; y < data.size(); y++) {
        for (int x = 0; x < data[y].size(); x++) {
            if (data[y][x] == '>' && at(data, x+1, y) == '.') {
                dataCopy[y][x] = '.';
                x++;
                set(dataCopy, x, y, '>');
            }
        }
    }
    data = dataCopy;
}

void south(std::vector<std::string> &data) {
    std::vector<std::string> dataCopy = data;
    for (int x = 0; x < data[0].size(); x++) {
        for (int y = 0; y < data.size(); y++) {
            if (data[y][x] == 'v' && at(data, x, y+1) == '.') {
                dataCopy[y][x] = '.';
                y++;
                set(dataCopy, x, y, 'v');
            }
        }
    }
    data = dataCopy;
}

// I initially missed this: every sea cucumber in the herd *first simultaneously considers* whether there is a sea cucumber in the adjacent location it's facing (even another sea cucumber facing the same direction), and then every sea cucumber facing an empty location simultaneously moves into that location
// Once I read the problem more carefully, I added dataCopy
void step(std::vector<std::string> &data) {
    east(data);
    south(data);
}

int main(int argc, char** argv) {
    bool visualize = false;
    if (argc >= 2) {
        if (!strcmp(argv[1], "-v") || !strcmp(argv[1], "--visualize")) {
            visualize = true;
        }
        if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "-?") || !strcmp(argv[1], "--help")) {
            std::cout << "Help:" << std::endl;
            std::cout << "\t-v, --visualize: visualize sea cucumbers moving" << std::endl;
            std::cout << "\t-h, -?, --help: show this help page" << std::endl;
            std::cout << "\nHave a nice day!" << std::endl;
            return 0;
        }
    }
    std::vector<std::string> current = getStrings();
    std::vector<std::string> previous{};
    unsigned long s = 0;
    while (previous != current) {
        if (visualize) {
            std::cout << "\033[;H"; // clear screen
            std::cout << "Step " << s << std::endl;
            std::cout << std::endl;
            for (std::string line : current) {
                std::cout << line << std::endl;
            }
            delay(100);
        }
        previous = current;
        step(current);
        s++;
    }
    std::cout << "After " << s << " steps, the sea cucumbers stop moving" << std::endl;
    return 0;
}