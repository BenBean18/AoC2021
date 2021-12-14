#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>
#include <regex>
#include <set>

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
    int steps = 10;
    std::set<char> allCharacters;
    for (char c : polymer) {
        allCharacters.insert(c);
    }
    for (std::string s : ruleOutputs) {
        allCharacters.insert(s[0]);
    }
    for (int i = 0; i < steps; i++) {
        std::string pair = "  "; // this will always be 2 long
        std::string newPolymer = polymer;
        for (int i = 0; i < polymer.size(); i++) {
            char c = polymer[i];
            pair[0] = pair[1];
            pair[1] = c;
            auto foundPair = std::find(ruleInputs.begin(),ruleInputs.end(),pair);
            if (foundPair != ruleInputs.end()) {
                newPolymer.insert(i + newPolymer.size()-polymer.size(), ruleOutputs[foundPair-ruleInputs.begin()]);
            }
        }
        polymer = newPolymer;
    }
    std::vector<unsigned long> characterFrequencies;
    for (char c : allCharacters) {
        characterFrequencies.push_back(std::count(polymer.begin(), polymer.end(), c));
    }
    // for (auto i : characterFrequencies) {
    //     std::cout << i << " ";
    // }
    std::cout << *std::max_element(characterFrequencies.begin(), characterFrequencies.end()) - *std::min_element(characterFrequencies.begin(), characterFrequencies.end()) << std::endl;
    return 0;
}