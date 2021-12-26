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

std::vector<Operation> parseInput() {
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
    return ops;
}

int main(int argc, char** argv) {
    auto ops = parseInput();
    std::string num = "00000000000000";
    // Each digit has a z value that will cause it to be 0. Find highest possibility of w for all that will cause the next digit to produce a z that will cause the next digit to produce a z that will cause (...) the last digit to produce 0.
    // Might need to combine all ops into one function.
    // Reverse the equation for the last one. For all w values, find all z values that cause it to be 0. (aka instead of newZ = z and w and a bunch of stuff, 0 = z and w and a bunch of stuff -> w and a bunch of stuff = z required for it to be 0). Go up the line (e.g. for second one, instead of 0 = z and w and a bunch of stuff, do <all z values for last one to be 0> = z and w and a bunch of stuff).
    
    // no worky:
    // unsigned long long z = 0;
    // for (int i = 0; i < 14; i++) {
    //     for (int n = 1; n < 10; n++) {
    //         unsigned long long zCopy = ops[i].doOperation(z, n);
    //         if (zCopy == 0) { // This doesn't work because not all produce zero, we only need the last one to produce zero.
    //             if (n+'0' > num[i]+'0') {
    //                 num[i] = n+'0';
    //             }
    //             std::cout << "zero @ " << n << std::endl;
    //         }
    //     }
    //     z = ops[i].doOperation(z, num[i]);
    // }
    return 0;
}