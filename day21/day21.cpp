#include <iostream>
#include <fstream>
#include <vector>

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

class DeterministicDice {
    int nextRoll = 0;
public:
    int rolls = 0;
    int roll() {
        rolls++;
        if (nextRoll < 100) {
            nextRoll++;
            return nextRoll;
        } else {
            nextRoll = 1;
            return nextRoll;
        }
    }
};

class Player {
    int _square; // 0-9 instead of 1-10 so we can do modulo
    DeterministicDice *_dice;
public:
    int score = 0;
    Player(int square, DeterministicDice *dice) {
        this->_square = square - 1; // 0-9 so we can do modulo
        this->_dice = dice;
    }
    bool takeTurn() { // returns hasWon (true means won)
        int roll1 = _dice->roll();
        int roll2 = _dice->roll();
        int roll3 = _dice->roll();
        this->_square += roll1 + roll2 + roll3;
        this->_square %= 10;
        this->score += this->_square + 1; // 0-9 -> 1-10
        if (this->score >= 1000) {
            return true;
        }
        return false;
    }
};

int main(int argc, char** argv) {
    // Insert code here! Use snippets -- "strs" = strings, "ints" = ints, "oth" = other
    DeterministicDice dice = DeterministicDice();
    std::vector<std::string> strings = getStrings();
    int player1Start = strings[0][strings[0].size()-1] - '0';
    int player2Start = strings[1][strings[1].size()-1] - '0';
    Player player1(player1Start, &dice);
    Player player2(player2Start, &dice);
    bool player1Won = false;
    while (true) {
        if (player1.takeTurn()) {
            player1Won = true;
            break;
        }
        if (player2.takeTurn()) {
            player1Won = false;
            break;
        }
    }
    int losingScore;
    if (player1Won) {
        losingScore = player2.score;
    } else {
        losingScore = player1.score;
    }
    std::cout << losingScore * dice.rolls << std::endl;
    return 0;
}