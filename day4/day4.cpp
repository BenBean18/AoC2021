#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <algorithm>
#include <thread>
#include <atomic>
#include <sstream>
#include <regex>

std::array<int, 25> parseBingoBoard(std::string bingoBoard) {
    std::array<int, 25> board;
    bingoBoard = std::regex_replace(bingoBoard, std::regex("\\s+"), ",");
    std::stringstream bingoBoardStream = std::stringstream(bingoBoard);
    int index = 0;
    while (bingoBoardStream.good()) {
        std::string num;
        getline(bingoBoardStream, num, ',');
        board[index] = std::stoi(num);
        index++;
    }
    return board;
}

void checkCombination(std::array<int, 25> bingoBoard, std::vector<int> winningNumbers, std::array<int, 5> combination, bool &win) {
    bool correct = true;
    for (int i : combination) {
        correct = correct && (std::find(winningNumbers.begin(), winningNumbers.end(), bingoBoard[i]) != winningNumbers.end());
    }
    win = win || correct;
}

bool hasWon(std::array<int, 25> bingoBoard, std::vector<int> winningNumbers) {
    std::vector<std::array<int, 5> > winningCombinations{{0,1,2,3,4},{5,6,7,8,9},{10,11,12,13,14},{15,16,17,18,19},{20,21,22,23,24}, /*rows*/\
    {0,5,10,15,20},{1,6,11,16,21},{2,7,12,17,22},{3,8,13,18,23},{4,9,14,19,24},/*columns*/\
    {0,6,12,18,24},{4,8,12,16,20}/*diagonals*/};
    bool win = false;
    for (std::array<int, 5> c : winningCombinations) {
        checkCombination(bingoBoard, winningNumbers, c, win);
    }
    return win;
}

// Use multithreading to scan ~5 bingo boards in parallel

int main(int argc, char** argv) {
    // Insert code here! Use snippets -- "strs" = strings, "ints" = ints, "oth" = other
    std::string line;
    
    std::ifstream input_file("day4.txt");
    if (!input_file.is_open()) {
        std::cerr << "Error opening file" << std::endl;
        return {};
    }
    
    getline(input_file, line, '\n');
    std::stringstream stringStream(line);
    std::vector<int> winningNumbers;
    while (stringStream.good()) {
        std::string num;
        getline(stringStream, num, ',');
        winningNumbers.push_back(std::stoi(num));
    }

    std::vector<int> calledNumbers;

    std::vector<std::array<int, 25> > boards;
    while (input_file.good()) {
        getline(input_file, line);
        std::string board;
        for (int i = 0; i < 5; i++) {
            getline(input_file, line);
            board += line;
            board += " ";
        }
        board = std::regex_replace(board, std::regex("^ +| +$|( ) +"), "$1");
        if (board.length() < 10) {
            break;
        }
        std::array<int, 25> bingoBoard = parseBingoBoard(board);
        boards.push_back(bingoBoard);
    }

    for (int i : winningNumbers) {
        calledNumbers.push_back(i);
        for (std::array<int, 25> board : boards) {
            if (hasWon(board, calledNumbers)) {
                std::cout << "BINGO!" << std::endl;
                int sumOfUnmarkedNumbers = 0;
                for (int j : board) {
                    std::cout << j << " ";
                    sumOfUnmarkedNumbers += (std::find(calledNumbers.begin(), calledNumbers.end(), j) == calledNumbers.end()) * j;
                }
                std::cout << std::endl << "Answer to puzzle: " << sumOfUnmarkedNumbers * calledNumbers.back() << std::endl;
                return 0;
            }
        }
    }

    return 0;
}