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
        signed long long z = rand();
        signed long long final = doOperation2(z, 8, true);
        std::cout << z << " " << final << std::endl;
        signed long long reversed = reverse(final, 8)[0];
        std::cout << "reversed: " << reversed << std::endl;
    }
    signed long long doOperation(signed long long z, int w) const {
        return ((z / uniqueNumbers[0]) * ((25 * ((z % 26 + uniqueNumbers[1]) != w)) + 1) + (w + uniqueNumbers[2]) * ((z % 26 + uniqueNumbers[1]) != w));
    }
    signed long long doOperation2(signed long long z, int w, bool debug = false) const {
        signed long long zFinal = z / uniqueNumbers[0];
        if (debug) {
            std::cout << "zf " << zFinal << std::endl;
        }
        if ((z % 26 + uniqueNumbers[1]) != w) {
            signed long long zBefore = zFinal;
            zFinal = zFinal * 26; // anything times 26 mod 26 is 0
            zFinal = zFinal + w + uniqueNumbers[2];
        }
        return zFinal;
    }
    std::vector<signed long long> reverse(signed long long finalZ, int w) const {
        std::vector<signed long long> possibilities;
        unsigned long long origZ;
        bool times26AndStuff = false;
        if ((finalZ - w - uniqueNumbers[2]) % 26 == 0) {
            origZ = ((finalZ - w - uniqueNumbers[2]) / 26) * uniqueNumbers[0];
            times26AndStuff = true;
        } else {
            origZ = finalZ * uniqueNumbers[0];
        }
        for (int i = 0; i < uniqueNumbers[0]; i++) { // With "* uniqueNumbers[0]", we are reversing "originalZ / uniqueNumbers[0]" *rounded down*. This means that we need to consider all possibilities from z * uniqueNumbers[0] to z*(uniqueNumbers[0]*2-1).
            // To illustrate this with an example, to solve x/5 = 1 with no rounding down, the answer is 5. However, if we round down, 5/5, 6/5, 7/5, 8/5, and 9/5 are all valid.
            // I was close to figuring this out on my own, and https://www.reddit.com/r/adventofcode/comments/rnejv5/2021_day_24_solutions/hpu84cj/ gave me the last bit of knowledge I needed.

            // Another thing I missed is that (finalZ - w - uniqueNumbers[2]) % 26 != 0 does not guarantee that
            if (times26AndStuff != (((origZ + i) % 26 + uniqueNumbers[1]) == w)) {
                possibilities.push_back(origZ + i);
            }
        }
        for (auto p : possibilities) {
            // std::cout << p << " -> " << doOperation(p, w) << (doOperation(p, w)==finalZ?" == ":" != ") << finalZ << std::endl;
            assert(doOperation2(p, w) == finalZ);
        }
        return possibilities;
    }
    // Function that takes desired z and returns all w&(z%26,z/num) pairs that return that
    std::vector<std::pair<int, signed long long>> wZPairs(signed long long desiredZ) const { // idea: have a list of zs that would be suitable
        std::vector<std::pair<int, signed long long>> wz;
        for (int w = 1; w < 10; w++) {
            for (auto z : reverse(desiredZ, w)) {
                wz.push_back({w,z});
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

bool calculatePairs(int i, std::vector<Operation> &ops, std::array<int, 14> &starts, std::array<std::vector<std::pair<int, signed long long>>, 14> &wzPairs, std::array<int, 14> &number, bool debug = false) {
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

std::map<std::pair<long long, int>,unsigned char> zAndDigitToW;

// start at second last digit. compute all w and z pairs, using the zs and ws of the next digit. store them and their parents (parents in form pair<digit, index>), indexed by digit. go up one.
void recurseMe(const std::vector<Operation> &ops, std::array<std::vector<std::pair<std::pair<int, long long>, std::pair<int, int>>>, 14> &validWZs, int digit = 12) { // validWZs length = 14
    std::vector<std::pair<std::pair<int, long long>, std::pair<int, int>>> newValidWZs;
    std::vector<std::pair<std::pair<int, long long>, std::pair<int, int>>> oldValidWZs; // remove valid wzs of the next one that don't result in any wz pair above
    for (int i = 0; i < validWZs[digit+1].size(); i++) {
        auto wzPairs = ops[digit].wZPairs(validWZs[digit+1][i].first.second);
        for (auto p : wzPairs) {
            newValidWZs.push_back({p, {digit+1, i}});
            zAndDigitToW[{p.second,digit}] = p.first;
        }
        if (wzPairs.size() != 0) {
            oldValidWZs.push_back(validWZs[digit+1][i]);
        }
    }
    // validWZs[digit+1] = oldValidWZs;
    validWZs[digit] = newValidWZs;
    std::cout << newValidWZs.size() << std::endl;
    if (digit == 0) {
        return;
    }
    digit--;
    recurseMe(ops, validWZs, digit);
}

void followBack(std::array<std::vector<std::pair<std::pair<int, long long>, std::pair<int, int>>>, 14> &validWZs) {
    auto p = validWZs[0][validWZs[0].size()-1];
    std::cout << p.first.first;
    for (int digit = 1; digit < 14; digit++) {
        p = validWZs[p.second.first][p.second.second];
        std::cout << p.first.first;
    }
    std::cout << std::endl;
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
    // signed long long z = 0;
    // for (int i = 0; i < 14; i++) {
    //     for (int n = 1; n < 10; n++) {
    //         signed long long zCopy = ops[i].doOperation(z, n);
    //         if (zCopy == 0) { // This doesn't work because not all produce zero, we only need the last one to produce zero.
    //             if (n+'0' > num[i]+'0') {
    //                 num[i] = n+'0';
    //             }
    //             std::cout << "zero @ " << n << std::endl;
    //         }
    //     }
    //     z = ops[i].doOperation(z, num[i]);
    // }
    std::array<std::vector<std::pair<std::pair<int, signed long long>, std::pair<int,int>>>, 14> wzPairs;
    std::array<int, 14> number;
    for (auto p : ops[13].wZPairs(0)) {
        wzPairs[13].push_back({p, {-1,-1}});
    }
    // std::array<int, 14> starts;
    // starts.fill(1);
    // for (int i = 12; i >= 0; i--) {
    //     std::cout << i << " " << starts[i] << std::endl;
    //     bool foundNumber = false;
    //     for (auto i : number) {
    //         std::cout << i << " ";
    //     }
    //     std::cout << std::endl;
    //     while (!calculatePairs(i, ops, starts, wzPairs, number, true)) {
    //         int l = 12;
    //         starts[l]++;
    //         while (starts[l] > 8) {
    //             starts[l] = 1;
    //             l--;
    //             if (l < i) {
    //                 break;
    //             }
    //             starts[l]++;
    //         }
    //         calculatePairs(l, ops, starts, wzPairs, number);
    //         for (int k = 12; k >= i; k--) {
    //             std::cout << k << " " << starts[k] << "\t";
    //         }
    //         std::cout << "->" << calculatePairs(i, ops, starts, wzPairs, number) << std::endl;
    //     }
    //     // if didn't find a number, backtrack starting at outer level. decrease starting wz pair from last digit to current until pair is found
    // }
    recurseMe(ops, wzPairs);
    // for (auto i : wzPairs[0]) {
    //     std::cout << i.first.first << std::endl;
    // }
    followBack(wzPairs);
    // for (auto p : ops[ops.size()-1].wZPairs(0)) {
    //     std::cout << p.first << " " << p.second << std::endl;
    //     std::cout << ops[ops.size()-1].doOperation(p.second, p.first) << std::endl;
    // }
    return 0;
}