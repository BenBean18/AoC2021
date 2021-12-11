#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <map>
#include <sys/ioctl.h>
#include <unistd.h>
#include <chrono>
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

// Fast way to check if line is valid (that we can't use b/c we need the first character):
// check if there are the same amount of each starting character as the ending character

int main(int argc, char** argv) {
    struct winsize size;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
    std::vector<std::string> strings = getStrings();
    std::vector<char> startingCharacters{'(','[','{','<'};
    std::vector<char> closingCharacters{')',']','}','>'};
    std::map<char, char> closingToOpening = {{')','('},{']','['},{'}','{'},{'>','<'}};
    std::map<char, char> openingToClosing = {{'(',')'},{'[',']'},{'{','}'},{'<','>'}};
    std::map<char, int> scoreMap = {{')',1},{']',2},{'}',3},{'>',4}};
    std::vector<unsigned long long> scores{};
    std::cout << "\033[48;5;238m";
    for (int i = 0; i < size.ws_row; i++) {
        for (int j = 0; j < size.ws_col; j++) {
            std::cout << "\u200c";
        }
        std::cout << std::endl;
    }
    int counter = 0;
    double timeMultiplier = 6;
    for (std::string s : strings) {
        counter++;
        std::vector<int> calledOpeningIndices{};
        std::vector<char> calledOpeningCharacters{}; // This will store the opening characters we have encountered,
        // pushing to the back of the list. If we see a closing character, it must correspond to the last item in this vector.
        // If it does, the last item should be removed. If it doesn't, add to the score for the line.
        bool isIncomplete = true;
        for (int i = 0; i < s.size(); i++) {
            std::cout << "\033[" << 1 << "H\033[48;5;237m"; // move cursor to first row
            std::cout << counter << "/" << strings.size();
            std::cout << "\033[" << std::to_string(size.ws_row/2) << "H\033[48;5;237m"; // move cursor to middle
            char c = s[i];
            if (std::find(startingCharacters.begin(), startingCharacters.end(), c) != startingCharacters.end()) {
                calledOpeningCharacters.push_back(c);
                calledOpeningIndices.push_back(i);
                std::cout << "\033[" << 2 << "H\033[48;5;238m\r"; // move cursor to second row
                std::cout << "List:                   \rList: ";
                for (char c : calledOpeningCharacters) {
                    std::cout << c;
                    std::this_thread::sleep_for(std::chrono::milliseconds((int)(timeMultiplier*3)));
                    std::flush(std::cout);
                }
                std::cout << "\033[" << std::to_string(size.ws_row/2) << "H\033[48;5;237m"; // move cursor to middle
            } else if (std::find(closingCharacters.begin(), closingCharacters.end(), c) != closingCharacters.end()) {
                if (calledOpeningCharacters.size() == 0) {
                    std::cout << "\033[" << 2 << "H\033[48;5;238m\r"; // move cursor to second row
                    std::cout << "List:                   \rList: ";
                    for (char c : calledOpeningCharacters) {
                        std::cout << c;
                        std::this_thread::sleep_for(std::chrono::milliseconds((int)(timeMultiplier*3)));
                        std::flush(std::cout);
                    }
                    std::cout << "\033[" << std::to_string(size.ws_row/2) << "H\033[48;5;237m"; // move cursor to middle
                    std::cout << "\r\033[48;5;238m";
                    for (int i = 0; i < size.ws_col; i++) {
                        std::cout << " ";
                    }
                    std::cout << "\r\033[48;5;238m";
                    for (int i = 0; i < (size.ws_col - s.size())/2; i++) {
                        std::cout << " ";
                    }
                    std::cout << s.substr(0, i);
                    std::cout << "\033[48;5;197m" << s[i] << s.substr(i);
                    std::cout << "\033[48;5;238m";
                    std::flush(std::cout);
                    std::this_thread::sleep_for(std::chrono::milliseconds((int)(timeMultiplier*500)));
                    isIncomplete = false;
                    break;
                } else if (closingToOpening[c] != calledOpeningCharacters[calledOpeningCharacters.size()-1]) {
                    std::cout << "\033[" << 2 << "H\033[48;5;238m\r"; // move cursor to second row
                    std::cout << "List:                   \rList: ";
                    for (char c : calledOpeningCharacters) {
                        std::cout << c;
                        std::this_thread::sleep_for(std::chrono::milliseconds((int)(timeMultiplier*3)));
                        std::flush(std::cout);
                    }
                    std::cout << "\033[" << std::to_string(size.ws_row/2) << "H\033[48;5;237m"; // move cursor to middle
                    std::cout << "\r\033[48;5;238m";
                    for (int i = 0; i < size.ws_col; i++) {
                        std::cout << " ";
                    }
                    std::cout << "\r\033[48;5;238m";
                    for (int i = 0; i < (size.ws_col - s.size())/2; i++) {
                        std::cout << " ";
                    }
                    std::cout << s.substr(0, i);
                    std::cout << "\033[48;5;197m" << s[i] << s.substr(i);
                    std::cout << "\033[48;5;238m";
                    std::flush(std::cout);
                    std::this_thread::sleep_for(std::chrono::milliseconds((int)(timeMultiplier*500)));
                    isIncomplete = false;
                    break;
                } else {
                    std::cout << "\033[" << 2 << "H\033[48;5;238m\r"; // move cursor to second row
                    std::cout << "List:                   \rList: ";
                    for (char c : calledOpeningCharacters) {
                        std::cout << c;
                        std::this_thread::sleep_for(std::chrono::milliseconds((int)(timeMultiplier*3)));
                        std::flush(std::cout);
                    }
                    std::cout << "\033[" << std::to_string(size.ws_row/2) << "H\033[48;5;237m"; // move cursor to middle
                    calledOpeningCharacters.pop_back();
                    calledOpeningIndices.pop_back();
                }
            }
        }
        if (isIncomplete) {
            std::cout << "\033[" << 2 << "H\033[48;5;238m\r"; // move cursor to second row
            std::cout << "List:                   \rList: ";
            for (char c : calledOpeningCharacters) {
                std::cout << c;
                std::this_thread::sleep_for(std::chrono::milliseconds((int)(timeMultiplier*3)));
                std::flush(std::cout);
            }
            std::cout << "\033[" << std::to_string(size.ws_row/2) << "H\033[48;5;237m"; // move cursor to middle
            // for (int)
            std::cout << "\033[48;5;238m";
            int color = (rand() % 170 + 50);
            std::cout << s;
            std::cout << "\033[38;5;" << color << "m";
            unsigned long score = 0;
            std::string chars;
            std::this_thread::sleep_for(std::chrono::milliseconds((int)(timeMultiplier*100)));
            std::vector<int> alreadyCompletedOpenCharacterIndices{};
            for (int i = calledOpeningCharacters.size() - 1; i >= 0; i--) {
                ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
                score *= 5ULL;
                std::cout << "\r";
                for (int i = 0; i < (size.ws_col - (chars.size()+s.size()))/2; i++) {
                    std::cout << " ";
                }
                for (int j = 0; j < s.size(); j++) {
                    if (j == calledOpeningIndices[i]) {
                        std::cout << "\033[38;5;" << color << "m";
                        std::cout << s[j];
                        std::cout << "\033[0;m\033[48;5;238m";
                        alreadyCompletedOpenCharacterIndices.push_back(j);
                        // highlight
                    } else if (std::find(alreadyCompletedOpenCharacterIndices.begin(), alreadyCompletedOpenCharacterIndices.end(), j) != alreadyCompletedOpenCharacterIndices.end()) {
                        std::cout << "\033[0;m\033[48;5;238m\033[38;5;226m";
                        std::cout << s[j];
                    } else {
                        std::cout << "\033[0;m\033[48;5;238m";
                        std::cout << s[j];
                    }
                }
                std::cout << "\033[38;5;226m";
                std::cout << chars;
                std::cout << "\033[38;5;" << color << "m";
                std::cout << openingToClosing[calledOpeningCharacters[i]];
                chars += openingToClosing[calledOpeningCharacters[i]];
                std::cout << "\033[0;m\033[48;5;238m\r";
                std::flush(std::cout);
                std::this_thread::sleep_for(std::chrono::milliseconds((int)(timeMultiplier*50)));
                score += scoreMap[openingToClosing[calledOpeningCharacters[i]]];
            }
            for (int i = 0; i < size.ws_col - (s.size() + chars.size()); i++) {
                std::cout << " ";
            }
            std::cout << "\r";
            for (int i = 0; i < size.ws_col; i++) {
                std::cout << " ";
            }
            std::cout << "\r";
            std::cout << "\033[0;m";
            std::this_thread::sleep_for(std::chrono::milliseconds((int)(timeMultiplier*300)));
            scores.push_back(score);
        }

    }
    std::sort(scores.begin(), scores.end());
    unsigned long median = (scores[scores.size()/2ULL]);
    std::cout << median << std::endl;
    return 0;
}