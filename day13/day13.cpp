#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <regex>

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

bool orOperator(bool a, bool b) {
    return a || b;
}

class OrigamiPaper {
public:
    std::vector<std::vector<bool>> paper; // [y][x]

    OrigamiPaper() {

    }

    OrigamiPaper(std::vector<std::pair<int,int>> dots) {
        std::pair<int,int> size = {1+(*std::max_element(dots.begin(), dots.end(), [](std::pair<int,int> a, std::pair<int,int> b){ return a.first < b.first; })).first, 1+(*std::max_element(dots.begin(), dots.end(), [](std::pair<int,int> a, std::pair<int,int> b){ return a.second < b.second; })).second};
        paper = std::vector<std::vector<bool>>(size.second, std::vector<bool>(size.first));
        for (auto i : dots) {
            paper[i.second][i.first] = 1;
        }
    }

    void setDots(std::vector<std::pair<int,int>> dots) {
        std::pair<int,int> size = {1+(*std::max_element(dots.begin(), dots.end(), [](std::pair<int,int> a, std::pair<int,int> b){ return a.first < b.first; })).first, 1+(*std::max_element(dots.begin(), dots.end(), [](std::pair<int,int> a, std::pair<int,int> b){ return a.second < b.second; })).second};
        paper = std::vector<std::vector<bool>>(size.second, std::vector<bool>(size.first));
        for (auto i : dots) {
            paper[i.second][i.first] = 1;
        }
    }

    // Fold algorithm:
    // Starting at the bottom/right, go up/left until you hit the folding line, each time adding the line you iterate over to a new vector.
    //  (This effectively reverses the vector which we need to do, because of the way things are folded)
    // OR everything in the copied vector with the original vector, and resize the original vector to be the size minus the size of the copied vector

    // Edit: instead, copy from positive of the line to negative of the line
    // Example of edge case (. represents single fold, * represents double fold):
    /*
    ..***            *****
    ..*** instead of *****
    ..***            *****
    */

    // direction = 'x' or 'y', line = what x or y equals
    void fold(char direction, int line) {
        if (direction == 'y') {
            for (int y = 0; y < paper.size()-line; y++) {
                std::transform(paper[line+y].begin(), paper[line+y].end(), paper[line-y].begin(), paper[line-y].begin(), orOperator);
            }
            paper.resize(line);
        } else if (direction == 'x') {
            for (int y = 0; y < paper.size(); y++) {
                for (int x = 0; x < paper[0].size()-line; x++) {
                    paper[y][line-x] = paper[y][line-x] || paper[y][line+x];
                }
            }
            for (int y = 0; y < paper.size(); y++) {
                paper[y].resize(line);
            }
        }
    }
};

OrigamiPaper parseInputAndFold(int noLimit = false) {
    OrigamiPaper paper;
    std::vector<std::string> strings = getStrings();
    std::regex dot("(\\d+),(\\d+)");
    std::vector<std::pair<int,int>> dots;
    for (std::string s : strings) {
        std::smatch match;
        std::regex_match(s, match, dot);
        if (match.size() == 3) {
            dots.push_back({std::stoi(match[1].str()),std::stoi(match[2].str())});
        }
    }
    paper.setDots(dots);
    std::regex fold("fold along (.)=(\\d+)");
    int folds = 0;
    for (std::string s : strings) {
        std::smatch match;
        std::regex_match(s, match, fold);
        if ((match.size() == 3) && (folds < (noLimit * 100000 + 1))) {
            paper.fold(match[1].str()[0], std::stoi(match[2].str()));
            folds++;
        }
    }
    return paper;
}

int main(int argc, char** argv) {
    OrigamiPaper p = parseInputAndFold();
    int dotsOn = 0;
    for (std::vector<bool> line : p.paper) {
        for (bool b : line) {
            dotsOn += b;
        }
    }
    std::cout << dotsOn << " dots on" << std::endl;;
    return 0;
}