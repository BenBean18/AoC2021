#include <iostream>
#include <fstream>
#include <vector>
#include <any>
#include <variant>
#include <cmath>
#include <algorithm>

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

struct SnailfishNumberElement {
    int num;
    int level;
};

using SnailfishNumber = std::vector<SnailfishNumberElement>;

// use vector showing the int/pair and its parent, can determine recursion level fast (for explode) and see big numbers fast (for split)
// std::pair<std::any,std::any> addSnailfishNumbers(std::pair<std::any,std::any> a, std::pair<std::any,std::any> b) {
//     std::pair<std::any,std::any> num = std::pair<std::any, std::any>{a, b};
// }

SnailfishNumber reduceNumber(SnailfishNumber n) {
    SnailfishNumber num(n);
    bool needToReduce = true;
    while (needToReduce) { // this will do one empty iteration but that's ok
        needToReduce = false;
        for (int i = 0; i < num.size(); i++) {
            auto el = num[i];
            bool hasPair = false;
            try {
                if (num.at(i).level == num.at(i+1).level) {
                    hasPair = true;
                }
            } catch (...) {

            }
            if (el.level > 4 && hasPair) { // explode. this one was >= 4 before, which was an off by one error that took me a while to find
                // el is the first of the pair
                // (pair is [num[i] aka el, num[i+1]])
                // for (int j = 0; j < num.size(); j++) {
                //     if (j == i || j == i+1) {
                //         std::cout << "\033[48;5;197m";
                //     }
                //     std::cout << num[j].num << ":" << num[j].level << ",";
                //     if (j == i || j == i+1) {
                //         std::cout << "\033[0m";
                //     }
                // }
                // std::cout << std::endl << " exploding" << el.num << ": " << std::endl;
                // std::flush(std::cout);
                try {
                    num.at(i-1).num += el.num;
                } catch (...) {

                }
                try {
                    num.at(i+2).num += num[i+1].num;
                } catch (...) {
                    
                }
                num.erase(num.begin() + i+1);
                num.erase(num.begin() + i);
                el.num = 0;
                el.level -= 1;
                num.insert(num.begin()+i, el);
                // for (int j = 0; j < num.size(); j++) {
                //     std::cout << num[j].num << ":" << num[j].level << ",";
                // }
                // std::cout << std::endl << std::endl;
                // std::flush(std::cout);
                needToReduce = true;
                break;
            }
        }
        // this was my bug, I was splitting before exploding if there was a splittable number before an explodable one
        if (!needToReduce) { // if none exploded
            for (int i = 0; i < num.size(); i++) {
                auto el = num[i];
                if (el.num > 9) { // split
                    // split code
                    SnailfishNumberElement left = el;
                    SnailfishNumberElement right = el;
                    left.level += 1;
                    right.level += 1;
                    left.num = floor(((double)el.num)/2.0);
                    right.num = ceil(((double)el.num)/2.0);
                    // for (int j = 0; j < num.size(); j++) {
                    //     std::cout << num[j].num << ":" << num[j].level << ",";
                    // }
                    // std::cout << std::endl << " splitting: " << std::endl;
                    // std::flush(std::cout);
                    num.insert(num.begin()+i+1, left);
                    num.insert(num.begin()+i+2, right);
                    num.erase(num.begin()+i);
                    // for (int j = 0; j < num.size(); j++) {
                    //     std::cout << num[j].num << ":" << num[j].level << ",";
                    // }
                    // std::cout << std::endl << std::endl;
                    // std::flush(std::cout);
                    needToReduce = true;
                    break;
                }
            }
        }
    }
    return num;
}

SnailfishNumber addSnailfishNumbers(SnailfishNumber a, SnailfishNumber b) {
    SnailfishNumber num(a);
    num.insert(num.end(), b.begin(), b.end());
    for (int i = 0; i < num.size(); i++) {
        num[i].level += 1;
    }
    num = reduceNumber(num);
    return num;
}

std::vector<SnailfishNumber> parseInput() {
    int level = 0;
    std::vector<SnailfishNumber> lines;
    std::vector<std::string> strings = getStrings();
    for (std::string s : strings) {
        SnailfishNumber line;
        for (int i = 0; i < s.size(); i++) {
            if (s[i] == '[') {
                level++;
            } else if (s[i] == ']') {
                level--;
            } else if (s[i] == ',') {
                // do nothing
            } else {
                line.push_back({s[i]-'0', level});
            }
        }
        lines.push_back(line);
    }
    return lines;
}

unsigned long getMagnitude(SnailfishNumber n) { // thanks r/adventofcode, I could not figure out how to do this
    SnailfishNumber num(n);
    int iter = 0;
    while (num.size() > 1) {
        for (int i = 0; i < num.size()-1; i++) {
            if (num[i].level == num[i+1].level) {
                // is a pair
                num[i].num = num[i].num*3 + num[i+1].num*2;
                num[i].level -= 1;
                num.erase(num.begin() + i+1);
                break;
            }
        }
    }
    if (num.size() == 1) {
        return num[0].num;
    }
    return 0;
}

int main(int argc, char** argv) {
    auto data = parseInput();
    auto sum = addSnailfishNumbers(data[0], data[1]);
    unsigned long maxMagnitude = 0;
    for (int i = 0; i < data.size(); i++) {
        for (int j = 0; j < data.size(); j++) {
            sum = addSnailfishNumbers(data[i], data[j]);
            unsigned long magnitude = getMagnitude(sum);
            if (magnitude > maxMagnitude) {
                maxMagnitude = magnitude;
            }
        }
    }
    std::cout << maxMagnitude << std::endl;
    return 0;
}