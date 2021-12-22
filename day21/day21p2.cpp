#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
#include <cassert>

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

int rollCount = 0;

struct Game {
    unsigned int p1Square;
    unsigned int p2Square;
    unsigned int p1Score = 0;
    unsigned int p2Score = 0;
    bool p1Turn;
    int ws;
    Game(int p1, int p2, bool p1Turn, int winningScore = 21) {
        p1Square = p1;
        p2Square = p2;
        ws = winningScore;
        this->p1Turn = p1Turn;
    }
    bool hasAnyoneWon() {
        if (p1Score >= ws || p2Score >= ws) {
            return true;
        }
        return false;
    }
    Game nextState(int r) {
        Game g(*this);
        if (g.p1Turn) {
            g.p1Square += r;
            g.p1Square--;
            g.p1Square = g.p1Square % 10;
            g.p1Square++;
            g.p1Score += g.p1Square;
            g.p1Turn = false;
        } else {
            g.p2Square += r;
            g.p2Square--;
            g.p2Square = g.p2Square % 10;
            g.p2Square++;
            g.p2Score += g.p2Square;
            g.p1Turn = true;
        }
        return g;
    }
    unsigned long long hash();
};
unsigned long long hashStr(const char* s, unsigned long long salt)
{
    unsigned long long h = salt;
    while (*s)
        h = h * 101 + (unsigned long long) *s++;
    return h;
}
unsigned long long Game::hash() {
    std::string s{(char)p1Square, (char)p2Square, (char)p1Score, (char)p2Score, p1Turn};
    return hashStr(s.c_str(), 1234);
}
inline bool operator<(Game a, Game b) {
    return a.hash() < b.hash();
}

// can roll 3 (1/27),4 (3/27),5 (6/27),6 (7/27),7 (6/27),8 (3/27),9 (1/27) (7 total states)
std::vector<int> rolls{3, 4, 5, 4, 5, 6, 5, 6, 7, 4, 5, 6, 5, 6, 7, 6, 7, 8, 5, 6, 7, 6, 7, 8, 7, 8, 9};

using ull = unsigned long long;
std::map<Game, ull> states; // value = how many in that state, key = state
ull p1Count = 0;
ull p2Count = 0;

std::pair<ull, ull> roll(Game game) {
    states[game] = 1;
    while (std::count_if(states.begin(), states.end(), [](auto el){ return el.second > 0; }) > 0) {
        std::map<Game, ull> statesCopy{};
        for (auto el : states) {
            for (int r : rolls) {
                Game g = el.first;
                if (g.p1Turn) {
                    g.p1Square += r;
                    g.p1Square--;
                    g.p1Square = g.p1Square % 10;
                    g.p1Square++;
                    g.p1Score += g.p1Square;
                    g.p1Turn = false;
                    if (g.p1Score >= game.ws) {
                        p1Count += el.second;
                    } else {
                        statesCopy[g] += el.second;
                    }
                } else {
                    g.p2Square += r;
                    g.p2Square--;
                    g.p2Square = g.p2Square % 10;
                    g.p2Square++;
                    g.p2Score += g.p2Square;
                    g.p1Turn = true;
                    if (g.p2Score >= game.ws) {
                        p2Count += el.second;
                    } else {
                        statesCopy[g] += el.second;
                    }
                }
                assert(g.p1Turn != el.first.p1Turn);
            }
        }
        states = statesCopy;
    }
    return {p1Count, p2Count};
}

// Try the lanternfish approach (std::map<Game,unsigned long long> where value = how many Games corresponding to that value)
// Store whose turn it is in the game within Game
// While nonzero values exist:
//     Compute next game (or add to win counts and remove) for each Game
//     See day6p2.cpp

int main(int argc, char** argv) {
    std::vector<std::string> strings = getStrings();
    int player1Start = strings[0][strings[0].size()-1] - '0';
    int player2Start = strings[1][strings[1].size()-1] - '0';
    std::cout << player1Start << " and " << player2Start << std::endl;
    bool player1Won = false;
    auto res = roll(Game(player1Start, player2Start, true, 21));
    std::cout << res.first << " " << res.second << std::endl;
    return 0;
}