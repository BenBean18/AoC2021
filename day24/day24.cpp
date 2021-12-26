#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <functional>
#include <regex>
#include <cassert>

std::string string() {
    std::string s;
    std::string line;

    std::ifstream input_file("input.txt");
    if (!input_file.is_open()) {
        std::cerr << "Error opening file" << std::endl;
        return {};
    }

    while (getline(input_file, line)) {
        s += line + "\n";
    }

    input_file.close();
    return s;
}

int w, x, y, z = 0;

class Operation {
    std::array<int, 3> uniqueNumbers;
public:
    Operation(int a, int b, int c) {
        uniqueNumbers[0] = a;
        uniqueNumbers[1] = b;
        uniqueNumbers[2] = c;
    }
    unsigned long long doOperation(unsigned long long z, int w) const {
        return ((z / uniqueNumbers[0]) * ((25 * ((z % 26 + uniqueNumbers[1]) != w)) + 1) + (w + uniqueNumbers[2]) * ((z % 26 + uniqueNumbers[1]) != w));
    }
};

std::function<unsigned long long(std::string)> parseInput() { // returns function taking 14 digit number as a string and returning z
    std::string s = string();
    std::regex r("inp w\\nmul x 0\\nadd x z\\nmod x 26\\ndiv z (.+)\\nadd x (.+)\\neql x w\\neql x 0\\nmul y 0\\nadd y 25\\nmul y x\\nadd y 1\\nmul z y\\nmul y 0\\nadd y w\\nadd y (.+)\\nmul y x\\nadd z y");
    std::smatch sm;
    std::vector<Operation> ops;
    while(std::regex_search(s, sm, r))
    {
        if (sm.size() == 4) {
            ops.push_back(Operation(std::stoi(sm[1]),std::stoi(sm[2]),std::stoi(sm[3])));
        }
        s = sm.suffix();
    }
    assert(ops.size() == 14);
    return [ops](std::string in){ unsigned long long z = 0; for (int i = 0; i < 14; i++) { z = ops[i].doOperation(z, in[i]-'0'); } return z; };
}

int main(int argc, char** argv) {
    auto f = parseInput();
    std::cout << f("99999999999999");
    return 0;
}