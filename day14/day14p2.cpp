#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>
#include <regex>
#include <set>
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

std::tuple<std::string,std::vector<std::string>,std::vector<std::string>> parseInput() {
    std::vector<std::string> strings = getStrings();
    std::regex rule("(.+) -> (.+)");
    std::vector<std::string> ruleInputs;
    std::vector<std::string> ruleOutputs;
    std::string polymer;
    polymer = strings[0];
    for (int i = 1; i < strings.size(); i++) {
        std::string s = strings[i];
        std::smatch match;
        std::regex_match(s, match, rule);
        if (match.size() == 3) {
            ruleInputs.push_back(match[1].str());
            ruleOutputs.push_back(match[2].str());
        }
    }
    return {polymer, ruleInputs, ruleOutputs};
}

// Given that it asks for the frequency of the most common minus the least common, we can simulate it with math maybe

int main(int argc, char** argv) {
    std::vector<std::string> ruleInputs;
    std::vector<std::string> ruleOutputs;
    std::string polymer;
    std::tie(polymer, ruleInputs, ruleOutputs) = parseInput();
    int steps = 40;
    std::set<char> allCharacters;
    for (char c : polymer) {
        allCharacters.insert(c);
    }
    for (std::string s : ruleOutputs) {
        allCharacters.insert(s[0]);
    }
    std::map<std::string,unsigned long long> pairs;
    for (int i = 1; i < polymer.size(); i++) {
        std::string pair = ""; // this will always be 2 long
        pair.push_back(polymer[i-1]);
        pair.push_back(polymer[i]);
        pairs[pair] += 1;
    }
    for (int i = 0; i < steps; i++) {
        auto pairsCopy = pairs;
        for (int i = 0; i < ruleInputs.size(); i++) {
            auto ri = ruleInputs[i];
            auto ro = ruleOutputs[i];
            unsigned long long howManyPairs = pairs[ri];
            std::string pair1 = "";
            pair1.push_back(ri[0]);
            pair1.push_back(ro[0]);
            std::string pair2 = "";
            pair2.push_back(ro[0]);
            pair2.push_back(ri[1]);
            pairsCopy[ri] -= pairs[ri];
            pairsCopy[pair1] += howManyPairs;
            pairsCopy[pair2] += howManyPairs;
        }
        pairs = pairsCopy;
    }
    std::map<char, unsigned long long> characterFrequencyMap;
    for (auto pair : pairs) {
        // characterFrequencies.push_back(std::count(polymer.begin(), polymer.end(), c));
        characterFrequencyMap[pair.first[0]] += pair.second;
        characterFrequencyMap[pair.first[1]] += pair.second;
    }
    std::vector<unsigned long long> characterFrequencies;
    for (auto f : characterFrequencyMap) {
        std::cout << f.first << " " << f.second << std::endl; // these are double oddly
        characterFrequencies.push_back(f.second);
    }
    std::cout << (*std::max_element(characterFrequencies.begin(), characterFrequencies.end()) - *std::min_element(characterFrequencies.begin(), characterFrequencies.end())) / 2 + 1 << std::endl;
    return 0;
}