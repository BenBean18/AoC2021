#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <algorithm>
#include <regex>

struct Point {
    int x;
    int y;
    int z;
};
inline Point operator+(Point a, Point b) {
    return {a.x+b.x, a.y+b.y, a.z+b.z};
}
inline Point operator-(Point a, Point b) {
    return {a.x-b.x, a.y-b.y, a.z-b.z};
}
inline bool operator<(Point a, Point b) {
    return a.x < b.x;
}
std::ostream &operator<<(std::ostream &os, Point const &p) { 
    return os << p.x << "," << p.y << "," << p.z;
}
inline bool operator==(Point a, Point b) {
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

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

// I don't understand matrices and 3d rotation too much yet, so I used the code from https://stackoverflow.com/questions/16452383/how-to-get-all-24-rotations-of-a-3-dimensional-array and changed it to be C++
Point roll(Point v) { return {v.x,v.z,-v.y}; }
Point turn(Point v) { return {-v.y,v.x,v.z}; }
std::vector<Point> sequence(Point v) {
    std::vector<Point> rotations;
    for (int cycle = 0; cycle < 2; cycle++) {
        for (int step = 0; step < 3; step++) { // Yield RTTT 3 times
            v = roll(v);
            rotations.push_back(v);            //    Add R
            for (int i = 0; i < 3; i++) {      //    Add TTT
                v = turn(v);
                rotations.push_back(v);
            }
        }
        v = roll(turn(roll(v)));  // Do RTR
    }
    return rotations;
}

std::vector<Point> reverseSequence(Point v) {
    std::vector<Point> rotations;
    // for (int cycle = 0; cycle < 2; cycle++) {
    //     v = roll(turn(roll(v)));  // Do RTR
    //     for (int step = 0; step < 3; step++) { // Yield TTTr 3 times
    //         for (int i = 0; i < 3; i++) {      //    Add TTT
    //             v = turn(v);
    //             rotations.push_back(v);
    //         }
    //         v = roll(v);
    //         rotations.push_back(v);            //    Add R
    //     }
    // }
    auto orig = sequence(v);
    rotations = sequence(orig[orig.size()-1]); // make it loop
    return rotations;
}

std::vector<Point> relative(std::vector<Point> points, Point base) {
    std::vector<Point> relativePoints;
    for (Point p : points) {
        relativePoints.push_back(p - base);
    }
    return relativePoints;
}

struct Possibility {
    std::vector<Point> p; // possibility
    int r; // rotation
    Point b; // base
};

std::vector<Possibility> allPossibilities(std::vector<Point> points) { 
    std::vector<Possibility> possibilities; // 24 rotations * how many possible relative offsets
    for (auto &i : possibilities) {
        i.p.resize(points.size());
    }
    for (int rot = 0; rot < 24; rot++) {
        for (int base = 0; base < points.size(); base++) {
            std::vector<Point> possibility; // there will be 600 of these
            for (int p = 0; p < points.size(); p++) {
                auto pr = sequence(points[p]);
                auto br = sequence(points[base]);
                Point current = pr[rot] - br[rot];
                possibility.push_back(current);
            }
            possibilities.push_back({possibility, rot, points[base]});
        }
    }
    return possibilities;
}

std::vector<Point> inCommon(std::vector<Point> scanner1, std::vector<Point> scanner2) {
    auto sc1 = allPossibilities(scanner1);
    auto sc2 = allPossibilities(scanner2);
    int mostInCommon = 0;
    std::vector<Point> mostInCommonVec;
    for (auto p1 : sc1) {
        std::sort(p1.p.begin(), p1.p.end());
        for (auto p2 : sc2) {
            std::sort(p2.p.begin(), p2.p.end());
            std::vector<Point> inCommon(scanner1.size());
            auto it = std::set_intersection(p1.p.begin(), p1.p.end(), p2.p.begin(), p2.p.end(), inCommon.begin());
            inCommon.resize(it-inCommon.begin());
            if (inCommon.size() >= mostInCommon) {
                mostInCommon = inCommon.size();
                mostInCommonVec = inCommon;
                std::cout << p1.r << " " << p1.b << " and " << p2.r << " " << p2.b << ". 1 relative to 0: " << reverseSequence(p2.b)[p2.r] - reverseSequence(p1.b)[p1.r] << std::endl; // x and y are flipped
                for (auto &p : mostInCommonVec) {
                    p = reverseSequence(p - p2.b)[p2.r];
                }
            }
        }
    }
    return mostInCommonVec;
}

std::vector<std::vector<Point>> parseInput() {
    std::vector<std::vector<Point>> scanners;
    std::vector<std::string> strings = getStrings();
    std::regex beacon("([\\d-]+),([\\d-]+),([\\d-]+)");
    int index = -1;
    std::vector<Point> thisScanner;
    for (std::string s : strings) {
        if (s[0] == '-' && s[1] == '-' && s[2] == '-') {
            if (s != "--- scanner 0 ---") {
                scanners.push_back(thisScanner);
            }
            index++;
            thisScanner = {};
        } else {
            std::smatch match;
            std::regex_match(s, match, beacon);
            if (match.size() == 4) {
                thisScanner.push_back({std::stoi(match[1]),std::stoi(match[2]),std::stoi(match[3])});
            }
        }
    }
    scanners.push_back(thisScanner);
    return scanners;
}

int main(int argc, char** argv) {
    auto scanners = parseInput();
    auto common = inCommon(scanners[0], scanners[1]);
    for (Point p : common) {
        std::cout << p + scanners[1][0] << std::endl;
    }
    return 0;
}