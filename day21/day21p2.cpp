#include <iostream>
#include <fstream>
#include <vector>
#include <thread>

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

bool p1Turn = true;
int rollCount = 0;

struct Game {
    int p1Square;
    int p2Square;
    int p1Score = 0;
    int p2Score = 0;
    Game(int p1, int p2) {
        p1Square = p1;
        p2Square = p2;
    }
};

// can roll 3 (1/27),4 (3/27),5 (6/27),6 (7/27),7 (6/27),8 (3/27),9 (1/27) (7 total states)
std::vector<int> rolls{3, 4, 5, 4, 5, 6, 5, 6, 7, 4, 5, 6, 5, 6, 7, 6, 7, 8, 5, 6, 7, 6, 7, 8, 7, 8, 9};
std::vector<std::thread> threads;

unsigned long long roll(Game game, unsigned long long &p1Count, unsigned long long &p2Count, unsigned long long recursionDepth = 0) {
    for (int r1 = 1; r1 <= 3; r1++) {
        Game game1 = game;
        for (int r2 = 1; r2 <= 3; r2++) {
            Game game2 = game1;
            for (int r3 = 1; r3 <= 3; r3++) {
                Game g = game2;
                if (p1Turn) {
                    g.p1Square += r1+r2+r3;
                    g.p1Square %= 10;
                    g.p1Score += g.p1Square + 1;
                    p1Turn = false;
                    if (g.p1Score >= 21) {
                        p1Count += 1;
                        g.p1Score = 0;
                    } else {
                        recursionDepth++;
                        if (recursionDepth < 5) {
                            threads.emplace_back(roll, g, std::ref(p1Count), std::ref(p2Count), recursionDepth);
                        } else {
                            roll(g, p1Count, p2Count, recursionDepth);
                        }
                    }
                } else {
                    g.p2Square += r1+r2+r3;
                    g.p2Square %= 10;
                    g.p2Score += g.p2Square + 1;
                    p1Turn = true;
                    if (g.p2Score >= 21) {
                        p2Count += 1;
                        g.p2Score = 0;
                    } else {
                        recursionDepth++;
                        if (recursionDepth < 5) {
                            threads.emplace_back(roll, g, std::ref(p1Count), std::ref(p2Count), recursionDepth);
                        } else {
                            roll(g, p1Count, p2Count, recursionDepth);
                        }
                    }
                }
            }
        }
    }
    return std::max(p1Count, p2Count);
}

int main(int argc, char** argv) {
    std::vector<std::string> strings = getStrings();
    int player1Start = strings[0][strings[0].size()-1] - '0';
    int player2Start = strings[1][strings[1].size()-1] - '0';
    unsigned long long p1c;
    unsigned long long p2c;
    bool player1Won = false;
    std::cout << roll(Game(player1Start, player2Start), p1c, p2c) << std::endl;
    return 0;
}