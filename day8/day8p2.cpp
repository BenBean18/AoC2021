#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <regex>

std::vector<std::string> getStrings() {
    std::vector<std::string> strings;
    std::string line;

    std::ifstream input_file("day8.txt");
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

    std::ifstream input_file("day8.txt");
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

std::vector<std::pair<std::array<std::string, 10>, std::array<std::string, 4>>> parseInput() {
    std::vector<std::string> strings = getStrings();
    std::regex sevenSegmentRegex("([^\\s]+) ([^\\s]+) ([^\\s]+) ([^\\s]+) ([^\\s]+) ([^\\s]+) ([^\\s]+) ([^\\s]+) ([^\\s]+) ([^\\s]+) \\| ([^\\s]+) ([^\\s]+) ([^\\s]+) ([^\\s]+)");
    std::vector<std::pair<std::array<std::string, 10>, std::array<std::string, 4>>> displays;

    for (std::string s : strings) {
        std::pair<std::array<std::string, 10>, std::array<std::string, 4>> line;
        std::smatch match;
        std::regex_match(s, match, sevenSegmentRegex);
        if (match.size() == 15) {   
            for (int i = 0; i < 10; i++) {
                line.first[i] = match[i+1].str();
                // std::cout << line.first[i] << " ";
            }
            // std::cout << "| ";
            for (int i = 0; i < 4; i++) {
                line.second[i] = match[i+11].str();
                // std::cout << line.second[i] << " ";
            }
            displays.push_back(line);
            // std::cout << std::endl;
        }
    }
    return displays;
}

std::vector<std::vector<char>> displayNumbers{{'a','b','c','e','f','g'},{'c','f'},{'a','c','d','e','g'},{'a','c','d','f','g'},{'b','c','d','f'},{'a','b','d','f','g'},{'a','b','d','e','f','g'},{'a','c','f'},{'a','b','c','d','e','f','g'},{'a','b','c','d','f','g'}};

// See planning.txt for reference

int deduceOutput(std::pair<std::array<std::string, 10>, std::array<std::string, 4>> display) {
    std::array<std::string, 10> uniqueValues;
    std::array<std::string, 4> output;
    uniqueValues = display.first;
    output = display.second;
    std::map<char, char> realToSeen; // key: the non-scrambled value, value: scrambled (inputted) value
    std::string one;
    std::string four;
    std::string seven;
    std::string eight;
    std::vector<std::string> sixLong{};
    std::vector<std::string> fiveLong{};
    for (std::string v : uniqueValues) {
        switch (v.size()) {
            case 2:
                one = v;
                break;
            case 3:
                seven = v;
                break;
            case 4:
                four = v;
                break;
            case 5:
                fiveLong.push_back(v);
                break;
            case 6:
                sixLong.push_back(v);
                break;
            case 7:
                eight = v;
                break;
            default:
                break;
        }
    }

    // see planning.txt

    realToSeen['a'] = seven[seven.find_first_not_of(one)];

    std::string sixLongWithAAndOneOfTheOnesInOne;
    for (std::string s : sixLong) {
        if (s.find_first_of(realToSeen['a']) != std::string::npos) {
            if ((s.find_first_of(one[0]) != std::string::npos) ^ (s.find_first_of(one[1]) != std::string::npos)) {
                sixLongWithAAndOneOfTheOnesInOne = s;
            }
        }
    }
    realToSeen['c'] = one[one.find_first_not_of(sixLongWithAAndOneOfTheOnesInOne)];

    realToSeen['f'] = one[one.find_first_not_of(realToSeen['c'])];

    std::string fiveLongWithACF;
    for (std::string s : fiveLong) {
        if (s.find_first_of(realToSeen['a']) != std::string::npos && s.find_first_of(realToSeen['c']) != std::string::npos && s.find_first_of(realToSeen['f']) != std::string::npos) {
            fiveLongWithACF = s;
        }
    }
    realToSeen['g'] = fiveLongWithACF[fiveLongWithACF.find_first_not_of(four + realToSeen['a'])];

    std::string fiveLongWithACGNotF;
    for (std::string s : fiveLong) {
        if (s.find_first_of(realToSeen['a']) != std::string::npos && s.find_first_of(realToSeen['c']) != std::string::npos && s.find_first_of(realToSeen['g']) != std::string::npos && s.find_first_of(realToSeen['f']) == std::string::npos) {
            fiveLongWithACGNotF = s;
        }
    }
    realToSeen['e'] = fiveLongWithACGNotF[fiveLongWithACGNotF.find_first_not_of(four + realToSeen['a'] + realToSeen['c'] + realToSeen['g'])];

    std::string sixLongWithACFGB;
    for (std::string s : sixLong) {
        if (s.find_first_of(realToSeen['a']) != std::string::npos && s.find_first_of(realToSeen['c']) != std::string::npos && s.find_first_of(realToSeen['f']) != std::string::npos && s.find_first_of(realToSeen['g']) != std::string::npos && s.find_first_of(realToSeen['e']) != std::string::npos) {
            sixLongWithACFGB = s;
        }
    }
    realToSeen['b'] = sixLongWithACFGB[sixLongWithACFGB.find_first_not_of((std::string)"" + realToSeen['a'] + realToSeen['c'] + realToSeen['f'] + realToSeen['g'] + realToSeen['e'])];

    std::string abcdefg = "abcdefg";
    realToSeen['d'] = abcdefg[abcdefg.find_first_not_of((std::string)"" + realToSeen['a'] + realToSeen['e'] + realToSeen['c'] + realToSeen['b'] + realToSeen['f'] + realToSeen['g'])];

    // Should work!

    // flip realToSeen to seenToReal
    std::map<char, char> seenToReal;

    for (std::map<char, char>::iterator i = realToSeen.begin(); i != realToSeen.end(); ++i) {
        seenToReal[i->second] = i->first;
        std::cout << i->first << ":" << i->second << " ";
    }
    std::cout << std::endl;
    
    // Get outputs, pass through seenToReal, sort alphabetically, compare to displayNumbers

    std::array<int, 4> realOutputs{};
    for (int i = 0; i < output.size(); i++) {
        std::vector<char> realOutput{};
        for (char c : output[i]) {
            realOutput.push_back(seenToReal[c]);
            std::cout << seenToReal[c];
        }
        std::cout << " ";
        std::sort(realOutput.begin(), realOutput.end()); // sort alphabetically
        realOutputs[i] = std::find(displayNumbers.begin(), displayNumbers.end(), realOutput) - displayNumbers.begin();
    }
    return realOutputs[0] * 1000 + realOutputs[1] * 100 + realOutputs[2] * 10 + realOutputs[3] * 1;
}

int main(int argc, char** argv) {
    auto displays = parseInput();
    unsigned long sum = 0;
    for (auto display : displays) {
        int out = deduceOutput(display);
        sum += out;
        std::cout << out << std::endl;
    }
    std::cout << sum << std::endl;
    return 0;
}