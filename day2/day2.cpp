#include <iostream>
#include <fstream>
#include <vector>

int main(int argc, char** argv) {
    int horizontalPosition = 0;
    int depth = 0;
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
                break;
            case 'd':
                depth += line[line.size()-1] - '0';
                break;
            case 'u':
                depth -= line[line.size()-1] - '0';
                break;
            default:
                break;
        }
    }

    input_file.close();
    std::cout << horizontalPosition * depth << std::endl;
    return 0;
}