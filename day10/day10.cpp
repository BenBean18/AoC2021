#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
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

// Fast way to check if line is valid (that we can't use b/c we need the first character):
// check if there are the same amount of each starting character as the ending character

int main(int argc, char** argv) {
    std::vector<std::string> strings = getStrings();
    std::vector<char> startingCharacters{'(','[','{','<'};
    std::vector<char> closingCharacters{')',']','}','>'};
    std::map<char, char> closingToOpening = {{')','('},{']','['},{'}','{'},{'>','<'}};
    std::map<char, int> scoreMap = {{')',3},{']',57},{'}',1197},{'>',25137}};
    unsigned long score = 0;
    for (std::string s : strings) {
        std::vector<char> calledOpeningCharacters{}; // This will store the opening characters we have encountered,
        // pushing to the back of the list. If we see a closing character, it must correspond to the last item in this vector.
        // If it does, the last item should be removed. If it doesn't, add to the score for the line.
        for (int i = 0; i < s.size(); i++) {
            char c = s[i];
            if (std::find(startingCharacters.begin(), startingCharacters.end(), c) != startingCharacters.end()) {
                calledOpeningCharacters.push_back(c);
            } else if (std::find(closingCharacters.begin(), closingCharacters.end(), c) != closingCharacters.end()) {
                if (calledOpeningCharacters.size() == 0) {
                    score += scoreMap[c];
                    break;
                } else if (closingToOpening[c] != calledOpeningCharacters[calledOpeningCharacters.size()-1]) {
                    score += scoreMap[c];
                    break;
                } else {
                    calledOpeningCharacters.pop_back();
                }
            }
        }
    }
    std::cout << score << std::endl;
    return 0;
}