#include <iostream>
#include <fstream>
#include <vector>
#include <functional>
#include <cmath>
#include <regex>

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

auto absComp = [](int a, int b){ return (abs(a)<abs(b)); };

struct Target {
    int x0; // min
    int y0;
    int x1; // max
    int y1;
    Target(int x0, int y0, int x1, int y1, bool abs = false) {
        if (abs) {
            this->x0 = std::min(x0, x1, absComp);
            this->y0 = std::min(y0, y1, absComp);
            this->x1 = std::max(x0, x1, absComp);
            this->y1 = std::max(y0, y1, absComp);
        } else {
            this->x0 = std::min(x0, x1);
            this->y0 = std::min(y0, y1);
            this->x1 = std::max(x0, x1);
            this->y1 = std::max(y0, y1);
        }
    }
};

double model(int t, double vel) {
    return -0.5*((double)t)*(double)(t-1) + ((double)vel)*(double)t;
}

int sign(int value) {
    return value>=0?1:-1;
}

std::tuple<bool,int> isInBounds(Target target, int xVel, int yVel) { // vel(ocity) = rise/run. Returns {inBounds, maxHeight}
    int maxHeight = 0;
    bool xZero = false;
    double x = 0;
    double y = 0;
    for (int t = 1; t < 500; t++) { // 500 may need to be increased, should change to smartly return (see line 95)
        x += xVel;
        if (abs(xVel) > 0) {
            xVel -= sign(xVel);
        }

        double y = model(t, yVel);

        if (y > maxHeight) {
            maxHeight = y;
        }

        if (target.x0 <= x && x <= target.x1 && target.y0 <= y && y <= target.y1) {
            return {true, maxHeight};
        }

        // a better way to early return that I can't get to work
        // if (abs(x) > abs(std::max(target.x0,target.x1,absComp)) || abs(y) > abs(std::max(target.y0,target.y1,absComp))) {
        //     return {false, 0};
        // }
    }
    return {false, 0};
}

int main(int argc, char** argv) {
    std::vector<std::string> strings = getStrings();
    std::smatch match;
    std::regex_match(strings[0], match, std::regex("target area: x=([\\d-]+)\\.\\.([\\d-]+), y=([\\d-]+)\\.\\.([\\d-]+)"));
    Target t(std::stoi(match[1].str()),std::stoi(match[3].str()),std::stoi(match[2].str()),std::stoi(match[4].str()));
    Target tAbs(std::stoi(match[1].str()),std::stoi(match[3].str()),std::stoi(match[2].str()),std::stoi(match[4].str()), true); // this gives us min and max based on absolute value
    int velocities = 0;
    int maxHeight = 0;
    for (int x = -abs(tAbs.x1); x <= abs(tAbs.x1); x++) {
        for (int y = -abs(tAbs.y1); y <= abs(tAbs.y1); y++) {
            bool inBounds;
            int height;
            std::tie(inBounds, height) = isInBounds(t, x, y);
            if (inBounds) {
                velocities += 1;
                if (height > maxHeight) {
                    maxHeight = height;
                }
            }
        }
    }
    std::cout << "Part 1: " << maxHeight << std::endl;
    std::cout << "Part 2: " << velocities << std::endl;
    return 0;
}