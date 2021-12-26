#include <iostream>
#include <fstream>
#include <vector>
#include <regex>
#include <thread>

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

// .intel_syntax noprefix
// ^ dest, src
// w = %r8
// x = %r9
// y = %r10
// z = %r11
// inp a -> pop a
// add a b -> add a, b
// mul a b -> imul a, b
// div a b -> xor %rdx, %rdx; mov %rax, a; idiv b; mov a, %rax;
// mod a b -> xor %rdx, %rdx; mov %rax, a; idiv b; mov a, %rdx;
// eql a b -> cmp a, b; sete a;
// end of program -> ret

// https://stackoverflow.com/a/24315631
std::string replaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

std::string assembly() {
    std::string s = string();
    s = replaceAll(s, "w", "%r8");
    s = replaceAll(s, "x", "%r9");
    s = replaceAll(s, "y", "%r10");
    s = replaceAll(s, "z", "%r11");
    s = std::regex_replace(s, std::regex("inp (.+)"), "pop $1");
    s = std::regex_replace(s, std::regex("add (.+) (.+)"), "add $1, $2");
    s = std::regex_replace(s, std::regex("mul (.+) (.+)"), "imul $1, $2");
    s = std::regex_replace(s, std::regex("div (.+) (.+)"), "xor %rdx, %rdx\nmov %rax, $1\nidiv $2\nmov $1, %rax");
    s = std::regex_replace(s, std::regex("mod (.+) (.+)"), "xor %rdx, %rdx\nmov %rax, $1\nidiv $2\nmov $1, %rdx");
    s = std::regex_replace(s, std::regex("eql (.+) (.+)"), "cmp $1, $2\nsete $1");
    s = std::regex_replace(s, std::regex(" ([-\\d]+)"), " $$$1"); // replace immediate with $immediate
    s += "ret";
    return s;
}

signed char convertToOperand(std::string s, bool &isImmediate) {
    if (s[0] == '-' || (s[0] >= '0' && s[0] <= '9')) {
        isImmediate = true;
        return std::stoi(s);
    } else {
        isImmediate = false;
        return s[0];
    }
}

struct Operation {
    unsigned int opcode : 3; // index of operation: inp = 0, add = 1, mul = 2, div = 3, mod = 4, eql = 5
    signed char operands[2];
    bool isImmediate = false;
    Operation(std::string s) {
        if (s[1] == 'n') {
            opcode = 0;
            operands[0] = s[4]; // input, only 1 operation
        } else if (s[1] == 'd') {
            opcode = 1;
            operands[0] = s[4];
            operands[1] = convertToOperand(s.substr(6), isImmediate);
        } else if (s[1] == 'u') {
            opcode = 2;
            operands[0] = s[4];
            operands[1] = convertToOperand(s.substr(6), isImmediate);
        } else if (s[1] == 'i') {
            opcode = 3;
            operands[0] = s[4];
            operands[1] = convertToOperand(s.substr(6), isImmediate);
        } else if (s[1] == 'o') {
            opcode = 4;
            operands[0] = s[4];
            operands[1] = convertToOperand(s.substr(6), isImmediate);
        } else if (s[1] == 'q') {
            opcode = 5;
            operands[0] = s[4];
            operands[1] = convertToOperand(s.substr(6), isImmediate);
        }
    }
};

struct ALU {
    char alu[4] = "ALU";
    long w, x, y, z;
    long &refToVar(signed char c) {
        if (c == 'w') {
            return this->w;
        }
        if (c == 'x') {
            return this->x;
        }
        if (c == 'y') {
            return this->y;
        }
        if (c == 'z') {
            return this->z;
        }
    }
    // Number is passed in backwards
    void runCode(std::vector<char> inputs, std::vector<Operation> code) {
        w, x, y, z = 0;
        for (Operation o : code) {
            switch (o.opcode) {
                case 0:
                    refToVar(o.operands[0]) = inputs[inputs.size()-1];
                    inputs.pop_back();
                    break;
                case 1:
                    if (o.isImmediate) {
                        refToVar(o.operands[0]) += o.operands[1]; 
                    } else {
                        refToVar(o.operands[0]) += refToVar(o.operands[1]);
                    }
                    break;
                case 2:
                    if (o.isImmediate) {
                        refToVar(o.operands[0]) *= o.operands[1]; 
                    } else {
                        refToVar(o.operands[0]) *= refToVar(o.operands[1]);
                    }
                    break;
                case 3:
                    if (o.isImmediate) {
                        refToVar(o.operands[0]) /= o.operands[1]; 
                    } else {
                        refToVar(o.operands[0]) /= refToVar(o.operands[1]);
                    }
                    break;
                case 4:
                    if (o.isImmediate) {
                        refToVar(o.operands[0]) %= o.operands[1]; 
                    } else {
                        refToVar(o.operands[0]) %= refToVar(o.operands[1]);
                    }
                    break;
                case 5:
                    if (o.isImmediate) {
                        refToVar(o.operands[0]) = refToVar(o.operands[0])==o.operands[1]; 
                    } else {
                        refToVar(o.operands[0]) = refToVar(o.operands[0])==refToVar(o.operands[1]);
                    }
                    break;
            }
        }
    }
};

std::vector<Operation> parseInput() {
    std::vector<std::string> strings = getStrings();
    std::vector<Operation> ops;
    for (std::string s : strings) {
        ops.push_back(Operation(s));
    }
    return ops;
}

int main(int argc, char** argv) {
    auto ops = parseInput();
    int threadNum = 32;
    std::vector<ALU> alus;
    std::vector<std::thread> threads;
    bool finishedThreads[threadNum]; // actually a bool
    for (int i = 0; i < threadNum; i++) {
        alus.push_back(ALU());
    }
    for (int i = 0; i < threadNum; i++) {
        ALU *a = &(alus[i]);
        bool *finished = &(finishedThreads[i]);
        threads.emplace_back([a, ops, finished](){(*a).runCode({1,1,1,1,1 ,1,1,1,1,1 ,1,1,1,1}, ops); *finished = true; });
        // std::cout << alu.z << '\n';
    }
    for (std::thread &t : threads) {
        t.join();
    }
    for (int i = 0; i < threadNum; i++) {
        std::cout << alus[i].z << std::endl;
    }
    // std::flush(std::cout);
    return 0;
}