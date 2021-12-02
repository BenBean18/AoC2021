#include <iostream>
#include <fstream>
#include <vector>

int main(int argc, char** argv) {
    long long horizontalPosition = 0;
    long long depth = 0;
    long long aim = 0;
    std::string line;
    std::ifstream input_file("day2.txt");
    if (!input_file.is_open()) {
        std::cerr << "Error opening file" << std::endl;
        return {};
    }
    
    while (getline(input_file, line)) {
        switch (line[0]) {
            case 'f':
                horizontalPosition += line[line.size()-1] - '0';
                depth += aim * (line[line.size()-1] - '0'); // forgot parentheses the first time
                break;
            case 'd':
                aim += line[line.size()-1] - '0';
                break;
            case 'u':
                aim -= line[line.size()-1] - '0';
                break;
            default:
                break;
        }
    }

    input_file.close();
    std::cout << horizontalPosition * depth << std::endl;
    return 0;
}