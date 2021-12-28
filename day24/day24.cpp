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

// int w, x, y, z = 0;

class Operation {
    std::array<int, 3> uniqueNumbers;
public:
    Operation(int a, int b, int c) {
        uniqueNumbers[0] = a;
        uniqueNumbers[1] = b;
        uniqueNumbers[2] = c;
        unsigned long long z = rand();
        for (int i = 1; i < 10; i++) {
            unsigned long long z = rand();
            assert(doOperation(z, i) == doOperation2(z, i));
        }
        z = rand();
        unsigned long long final = doOperation2(z, 8);
        std::cout << z << " " << final << std::endl;
        unsigned long long reversed = reverse(final, 8)[0];
        std::cout << "reversed: " << reversed << std::endl;
        assert(reversed == z);
    }
    unsigned long long doOperation(unsigned long long z, int w) const {
        return ((z / uniqueNumbers[0]) * ((25 * ((z % 26 + uniqueNumbers[1]) != w)) + 1) + (w + uniqueNumbers[2]) * ((z % 26 + uniqueNumbers[1]) != w));
    }
    unsigned long long doOperation2(unsigned long long z, int w) const {
        unsigned long long zFinal = z / uniqueNumbers[0];
        if ((z % 26 + uniqueNumbers[1]) != w) {
            unsigned long long zBefore = zFinal;
            zFinal = zFinal * 26; // anything times 26 mod 26 is 0
            zFinal = zFinal + w + uniqueNumbers[2];
        }
        return zFinal;
    }
    std::vector<unsigned long long> reverse(unsigned long long finalZ, int w) const {
        if (finalZ - w - ((finalZ / 26) * 26) == uniqueNumbers[2]) {
            if (uniqueNumbers[0] != 26) {
                finalZ = (finalZ - w - uniqueNumbers[2]) / 26;
            } else {
                // there are now 25 (I think) possibilities because / 26 * 26 truncates things
                unsigned long long zPrediction = (finalZ - w - uniqueNumbers[2]);
                unsigned long long op = doOperation(zPrediction, w);
                std::cout << "op " << op << " finalZ " << finalZ << std::endl;
                std::vector<unsigned long long> possibilities;
                for (zPrediction; zPrediction < finalZ - w - uniqueNumbers[2] + 26; zPrediction++) {
                    if (zPrediction % 26 + uniqueNumbers[1] != w) {
                        possibilities.push_back(zPrediction);
                    }
                }
                return possibilities;
            }
        }
        return {finalZ * uniqueNumbers[0]};
    }
    // Function that takes desired z and returns all w&(z%26,z/num) pairs that return that
    std::vector<std::pair<int, unsigned long long>> wZPairs(unsigned long long desiredZ) const { // idea: have a list of zs that would be suitable
        std::vector<std::pair<int, unsigned long long>> wz;
        for (int w = 1; w < 10; w++) {
            for (unsigned long z = 0; z < 99999999; z++) {
                if (doOperation(z, w) == desiredZ) {
                    wz.push_back({w,z});
                    break;
                }
            }
        }
        return wz;
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

bool calculatePairs(int i, std::vector<Operation> &ops, std::array<int, 14> &starts, std::array<std::vector<std::pair<int, unsigned long long>>, 14> &wzPairs, std::array<int, 14> &number, bool debug = false) {
    if (debug) {
        std::cout << "hi!" << std::endl;
    }
    for (int p = wzPairs[i+1].size() - starts[i]; p >= 0; p--) {
        auto pairs = ops[i].wZPairs(wzPairs[i+1][p].second);
        if (pairs.size() != 0) {
            wzPairs[i] = pairs;
            number[i+1] = wzPairs[i+1][p].first;
            return true;
        }
    }
    return false;
}

int main(int argc, char** argv) {
    std::time_t t = std::time(0);   // get time now
    std::tm* now = std::localtime(&t);
    srand(now->tm_sec);
    auto ops = parseInput();
    std::string num = "00000000000000";
    // The last digit has a z value that will cause it to be 0. Find highest possibility of w for all that will cause the next digit to produce a z that will cause the next digit to produce a z that will cause (...) the last digit to produce 0.
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
    std::array<std::vector<std::pair<int, unsigned long long>>, 14> wzPairs;
    std::array<int, 14> number;
    wzPairs[13] = ops[13].wZPairs(0);
    std::array<int, 14> starts;
    starts.fill(1);
    for (int i = 12; i >= 0; i--) {
        std::cout << i << " " << starts[i] << std::endl;
        bool foundNumber = false;
        while (!calculatePairs(i, ops, starts, wzPairs, number, true)) {
            for (auto i : number) {
                std::cout << i << " ";
            }
            int l = 12;
            starts[l]++;
            while (starts[l] > 8) {
                starts[l] = 1;
                l--;
                if (l == i) {
                    break;
                }
                starts[l]++;
            }
            calculatePairs(l, ops, starts, wzPairs, number);
            for (int k = 12; k >= i; k--) {
                std::cout << k << " " << starts[k] << "\t";
            }
            std::cout << "->" << calculatePairs(i, ops, starts, wzPairs, number) << std::endl;
        }
        // if didn't find a number, backtrack starting at outer level. decrease starting wz pair from last digit to current until pair is found
    }
    for (auto i : number) {
        std::cout << i << std::endl;
    }
    // for (auto p : ops[ops.size()-1].wZPairs(0)) {
    //     std::cout << p.first << " " << p.second << std::endl;
    //     std::cout << ops[ops.size()-1].doOperation(p.second, p.first) << std::endl;
    // }
    return 0;
}