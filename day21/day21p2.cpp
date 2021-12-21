#include <iostream>
#include <fstream>
#include <vector>
#include <map>

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
inline bool operator<(Game a, Game b) {
    return (a.p1Square*40 + a.p2Square*30 + a.p1Score*20 + a.p2Score*10) < (b.p1Square*40 + b.p2Square*30 + b.p1Score*20 + b.p2Score*10);
}

// can roll 3 (1/27),4 (3/27),5 (6/27),6 (7/27),7 (6/27),8 (3/27),9 (1/27) (7 total states)
std::vector<int> rolls{3, 4, 5, 4, 5, 6, 5, 6, 7, 4, 5, 6, 5, 6, 7, 6, 7, 8, 5, 6, 7, 6, 7, 8, 7, 8, 9};

using ull = unsigned long long;
std::map<std::pair<Game, int>, int> memo; // value = int (winner), key=<Game (state before),int (roll)>
std::map<std::pair<Game, int>, bool> inMemo; // key = memo key, value = in memo or not

std::pair<ull, ull> roll(Game game, unsigned long long recursionDepth = 0) {
    std::pair<ull, ull> sum{0,0};
    for (int r : rolls) {
        Game g = game;
        if (!inMemo[{game,r}]) {
            if (p1Turn) {
                g.p1Square += r;
                g.p1Square %= 10;
                g.p1Score += g.p1Square + 1;
                p1Turn = false;
                if (g.p1Score >= 21) {
                    g.p1Score = 0;
                    sum.first++;
                    memo[{game,r}] = 1;
                    inMemo[{game,r}] = true;
                } else {
                    recursionDepth++;
                    auto res = roll(g, recursionDepth);
                    sum.first += res.first;
                    sum.second += res.second;
                }
            } else {
                g.p2Square += r;
                g.p2Square %= 10;
                g.p2Score += g.p2Square + 1;
                p1Turn = true;
                if (g.p2Score >= 21) {
                    g.p2Score = 0;
                    sum.second++;
                    memo[{game,r}] = 1;
                    inMemo[{game,r}] = true;
                } else {
                    recursionDepth++;
                    auto res = roll(g, recursionDepth);
                    sum.first += res.first;
                    sum.second += res.second;
                }
            }
        } else {
            auto res = memo[{game,r}];
            if (res == 1) {
                sum.first++;
            } else {
                sum.second++;
            }
        }
    }
    return sum;
}

int main(int argc, char** argv) {
    std::vector<std::string> strings = getStrings();
    int player1Start = strings[0][strings[0].size()-1] - '0';
    int player2Start = strings[1][strings[1].size()-1] - '0';
    bool player1Won = false;
    auto res = roll(Game(player1Start, player2Start));
    std::cout << res.first << " " << res.second << std::endl;
    return 0;
}