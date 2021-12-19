#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <algorithm>
#include <regex>
#include <map>
#include <functional>

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

std::map<std::pair<Point, int>, Point> savedRotations; // key: <point, rotation number>, value: original point

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

std::vector<int> backwardsLookup = {13,20,2,7,4,14,21,3,10,9,8,11,22,0,5,15,16,17,18,19,1,6,12,23}; // index is the rotation number, outputs original
// calculated with https://topaz.github.io/paste/#XQAAAQCtAAAAAAAAAAAzG8qmJVsp4xOVzbSuI96rVMNrl/d4yGbFM81vIeWVIpTaAsfsEYmBEI20zH7EgbMc44UohDMDVzxWmjwqiaaxpgMZ2gVHFeoh5oQzY9j4tS/dX+7g5ze9/GrgeB6++FA2fQiEM8dWOj5tvvVN+qf17mfwJAMiH3Nt8TFOfOAV14flaZ5f/7NgQAA=

// reverseSequence(point, rotationUsedForPoint) -> originalPoint
Point reverseSequence(Point v, int rot) {
    return sequence(v)[backwardsLookup[rot]];
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
                savedRotations[{points[p], rot}] = points[p];
                savedRotations[{points[base], rot}] = points[base];
                Point current = pr[rot] - br[rot];
                possibility.push_back(current);
            }
            possibilities.push_back({possibility, rot, points[base]});
        }
    }
    return possibilities;
}

// returns {inCommon, transformFunctionFromScanner2ToScanner1}
std::tuple<std::vector<Point>, std::function<Point(Point)>> inCommon(std::vector<Point> scanner1, std::vector<Point> scanner2) {
    auto sc2 = allPossibilities(scanner2);
    std::vector<Possibility> sc1;
    std::vector<Point> t;
    for (int i = 1; i < scanner1.size(); i++) {
        t.push_back(scanner1[i] - scanner1[0]); // index 0 is base
    }
    sc1 = {{t, 0, scanner1[0]}};
    std::vector<Point> toReturn(scanner1.size());
    int transformRotation;
    Point transformPosition;
    Point transformBase;
    for (auto p1 : sc1) { // scanner 1 is constant
        std::sort(p1.p.begin(), p1.p.end());
        for (int i = 0; i < sc2.size(); i++) { // scanner 2 is all possibilities
            // for each possibility: point = sequence(orig) - sequence(base)
            // for transformation:
            // orig is what we have, unrotated p2 point
            // point is what we want (corresponds to correctly rotated)
            // base is p2.b, unrotated p2 point
            // 
            auto p2 = sc2[i];
            std::sort(p2.p.begin(), p2.p.end());
            std::vector<Point> inCommon(scanner1.size());
            auto it = std::set_intersection(p1.p.begin(), p1.p.end(), p2.p.begin(), p2.p.end(), inCommon.begin());
            inCommon.resize(it-inCommon.begin()+1);
            if (inCommon.size() >= 12) {
                for (auto &p : inCommon) {
                    p = p + scanner1[0]; // index 0 is base
                }
                toReturn = inCommon;
                transformRotation = p2.r; // p2.r is the rotation from scanner2 (relative to base) to scanner1
                transformPosition = scanner1[0];
                transformBase = p2.b;
                Point t = {515,917,-361};
                std::cout << (sequence(t)[p2.r] - sequence(p2.b)[p2.r]) + scanner1[0] << std::endl; // this only works for the base, which is weird because it is the opposite of line 123
                return {toReturn, [transformRotation,transformPosition,transformBase](Point p){ return (sequence(p)[transformRotation] - sequence(transformBase)[transformRotation]) + transformPosition; }};
            }
        }
    }
    return {toReturn, [transformRotation,transformPosition](Point p){ return sequence(p)[transformRotation] + transformPosition; }};
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
    std::vector<Point> commonPts;
    std::function<Point(Point)> transform;
    std::tie(commonPts, transform) = common;
    for (Point p : commonPts) {
        std::cout << p << std::endl;
    }
    std::cout << std::endl << std::endl;
    for (Point p : scanners[1]) {
        std::cout << p << " -> " << transform(p) << std::endl;
    }
    return 0;
}