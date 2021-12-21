#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <tuple>
#include <cassert>
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

using Image = std::vector<std::vector<bool>>;

std::tuple<Image, std::array<bool, 512>> parseInput(int iterations = 2) { // Trim trailing newline from input first.
    std::vector<std::string> strings = getStrings();
    std::array<bool, 512> algorithm;
    Image image;
    assert(strings[0].size() == 512);
    for (int i = 0; i < strings[0].size(); i++) {
        algorithm[i] = strings[0][i]=='#'?1:0;
    }
    strings.erase(strings.begin()+0); // remove algorithm
    strings.erase(strings.begin()+0); // remove empty line after algorithm
    image.resize(strings.size() + iterations*6); // +3 border on each side, does not need to be infinite
    for (auto &row : image) {
        row.resize(strings[0].size() + iterations*6); // +3 border on each side, does not need to be infinite
    }
    for (int row = 0; row < strings.size(); row++) {
        for (int col = 0; col < strings[0].size(); col++) {
            image[row+iterations*3][col+iterations*3] = strings[row][col]=='#'?1:0;
        }
    }
    return {image, algorithm};
}

Image enhance(Image originalImage, std::array<bool, 512> algo, bool resize = true) {
    Image newImage;
    if (resize) {
        for (int y = 0; y < 3; y++) {
            newImage.push_back({});
            newImage[y].resize(originalImage.size()+4);
        }
    }
    for (int tlr = 0; tlr < originalImage.size()-2; tlr++) { // tlr stands for top left row, doing top left of 3x3 area
        newImage.push_back({});
        if (resize) {
            for (int x = 0; x < 3; x++) {
                newImage[tlr+3].push_back(0);
            }
        }
        size_t index = tlr;
        if (resize) {
            index += 3;
        }
        for (int tlc = 0; tlc < originalImage[0].size()-2; tlc++) { // tlc -> top left column
            std::string bitString;
            for (int y = 0; y < 3; y++) {
                for (int x = 0; x < 3; x++) {
                    bitString += originalImage[tlr+y][tlc+x]?'1':'0';
                }
            }
            std::bitset<9> b(bitString);
            newImage[index].push_back(algo[b.to_ulong()]);
        }
        if (resize) {
            for (int x = 0; x < 3; x++) {
                newImage[index].push_back(0);
            }
        }
    }
    if (resize) {
        for (int y = 0; y < 3; y++) {
            newImage.push_back({});
            newImage[newImage.size()-1].resize(originalImage.size()+4);
        }
    }
    return newImage;
}

int main(int argc, char** argv) {
    Image image;
    std::array<bool, 512> algorithm;
    std::tie(image, algorithm) = parseInput(50);
    Image enhanced = enhance(image, algorithm, false);
    for (int i = 0; i < 50-1; i++) {
        enhanced = enhance(enhanced, algorithm, false);
    }
    for (auto row : enhanced) {
        for (bool c : row) {
            std::cout << (c?'#':'.');
        }
        std::cout << std::endl;
    }
    unsigned long num = 0;
    for (auto row : enhanced) {
        num += std::count(row.begin(), row.end(), 1);
    }
    std::cout << num;
    return 0;
}