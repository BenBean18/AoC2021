#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <algorithm>
#include <regex>

struct Matrix {
    std::vector<std::vector<int>> d; // d is for data, that's good enough for me
    Matrix() {

    }
    Matrix(int rows, int columns) {
        d.resize(rows);
        for (auto &e : d) {
            e.resize(columns);
        }
    }
    Matrix(std::vector<std::vector<int>> _d) {
        d = _d;
    }
    std::vector<int>& operator[](size_t i) {
        return d.at(i);
    }
    std::pair<int, int> size() {
        return {d.size(),d[0].size()}; // rows and columns
    }
    int rows() {
        return this->size().first;
    }
    int columns() {
        return this->size().second;
    }
    Matrix transpose();
};
std::ostream &operator<<(std::ostream &os, const Matrix &c) {
    Matrix m = c;
    for (int r = 0; r < m.rows(); r++) {
        for (int c = 0; c < m.columns(); c++) {
            os << m[r][c] << " ";
        }
        os << std::endl;
    }
    return os;
}
Matrix Matrix::transpose() {
    Matrix m(*this);
    Matrix t(m.columns(), m.rows());
    for (int row = 0; row < m.columns(); row++) {
        for (int column = 0; column < m.rows(); column++) {
            t[row][column] = m[column][row];
        }
    }
    return t;
}

struct Point {
    int x;
    int y;
    int z;
    Point() {

    }
    Point(int x_, int y_, int z_) {
        x = x_;
        y = y_;
        z = z_;
    }
    // must be a 3x1 matrix
    Point(Matrix m) {
        x = m[0][0];
        y = m[1][0];
        z = m[2][0];
    }
    operator Matrix() {
        return Matrix({{x},{y},{z}});
    }
};
inline Point operator+(Point a, Point b) {
    return {a.x+b.x, a.y+b.y, a.z+b.z};
}
inline Point operator-(Point a, Point b) {
    return {a.x-b.x, a.y-b.y, a.z-b.z};
}
// inline bool operator<(Point a, Point b) {
//     return a.x < b.x;
// }
std::ostream &operator<<(std::ostream &os, Point const &p) { 
    return os << p.x << "," << p.y << "," << p.z;
}
inline bool operator==(Point a, Point b) {
    return a.x == b.x && a.y == b.y && a.z == b.z;
}
inline bool operator<(Point a, Point b) {
    return (a.x+a.y+a.z) < (b.x+b.y+b.z);
}

Matrix multiplyMatrices3x3And3x1(Matrix a3, Matrix b1) {
    Matrix c{};
    for (auto row : a3.d) {
        c.d.push_back({row[0]*b1.d[0][0] + row[1]*b1.d[1][0] + row[2]*b1.d[2][0]});
    }
    return c;
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

// https://www.reddit.com/r/adventofcode/comments/rk8qfd/2021_day_19math_some_helpful_matrices_for_solving/
std::vector<std::vector<std::vector<int>>> rotationMatrices = \
{{{1,0,0},{0,1,0},{0,0,1}},{{-1,0,0},{0,-1,0},{0,0,1}},{{-1,0,0},{0,1,0},{0,0,-1}},{{1,0,0},{0,-1,0},{0,0,-1}},\
{{-1,0,0},{0,0,1},{0,1,0}},{{1,0,0},{0,0,-1},{0,1,0}},{{1,0,0},{0,0,1},{0,-1,0}},{{-1,0,0},{0,0,-1},{0,-1,0}},\
{{0,-1,0},{1,0,0},{0,0,1}},{{0,1,0},{-1,0,0},{0,0,1}},{{0,1,0},{1,0,0},{0,0,-1}},{{0,-1,0},{-1,0,0},{0,0,-1}},\
{{0,1,0},{0,0,1},{1,0,0}},{{0,-1,0},{0,0,-1},{1,0,0}},{{0,-1,0},{0,0,1},{-1,0,0}},{{0,1,0},{0,0,-1},{-1,0,0}},\
{{0,0,1},{1,0,0},{0,1,0}},{{0,0,-1},{-1,0,0},{0,1,0}},{{0,0,-1},{1,0,0},{0,-1,0}},{{0,0,1},{-1,0,0},{0,-1,0}},
{{0,0,-1},{0,1,0},{1,0,0}},{{0,0,1},{0,-1,0},{1,0,0}},{{0,0,1},{0,1,0},{-1,0,0}},{{0,0,-1},{0,-1,0},{-1,0,0}}};

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

std::vector<Point> relative(std::vector<Point> points, Point base) {
    std::vector<Point> relativePoints;
    for (Point p : points) {
        relativePoints.push_back(p - base);
    }
    return relativePoints;
}

struct Possibility {
    std::vector<Point> p; // possibility
    Matrix r; // rotation
    Point b; // base
};

std::vector<Possibility> allPossibilities(std::vector<Point> points) { 
    std::vector<Possibility> possibilities; // 24 rotations * how many possible relative offsets
    for (auto &i : possibilities) {
        i.p.resize(points.size());
    }
    for (int rot = 0; rot < 24; rot++) {
        Matrix m = rotationMatrices[rot];
        for (int base = 0; base < points.size(); base++) {
            std::vector<Point> possibility; // there will be 600 of these
            for (int p = 0; p < points.size(); p++) {
                auto pr = Point(multiplyMatrices3x3And3x1(m, Matrix(points[p])));
                auto br = Point(multiplyMatrices3x3And3x1(m, Matrix(points[base])));
                Point current = pr - br;
                possibility.push_back(current);
            }
            possibilities.push_back({possibility, Matrix(m), points[base]});
        }
    }
    return possibilities;
}

// returns {inCommon, transformFunctionFromScanner2ToScanner1}
// was std::tuple<std::vector<Point>, std::function<Point(Point)>>
std::tuple<std::vector<Point>, std::function<Point(Point)>> inCommon(std::vector<Point> scanner1, std::vector<Point> scanner2) {
    auto sc2 = allPossibilities(scanner2);
    std::vector<Possibility> sc1;
    std::vector<Point> t;
    for (int i = 1; i < scanner1.size(); i++) {
        t.push_back(scanner1[i] - scanner1[0]); // index 0 is base
    }
    sc1 = {{t, Matrix(), scanner1[0]}};
    std::vector<Point> toReturn(scanner1.size());
    Matrix transformRotation;
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
            auto p2c = sc2[i];
            std::sort(p2.p.begin(), p2.p.end());
            std::vector<Point> inCommon(scanner1.size());
            auto it = std::set_intersection(p1.p.begin(), p1.p.end(), p2.p.begin(), p2.p.end(), inCommon.begin());
            inCommon.resize(it-inCommon.begin()+1);
            if (inCommon.size() >= 12) {
                for (auto &p : inCommon) {
                    p = p + scanner1[0]; // index 0 is base
                }
                toReturn = inCommon;
                transformRotation = p2c.r; // p2.r is the rotation from scanner2 (relative to base) to scanner1
                transformPosition = scanner1[0];
                transformBase = p2c.b;
                Point t = {515,917,-361};
                // std::cout << (sequence(t)[p2.r] - sequence(p2.b)[p2.r]) + scanner1[0] << std::endl; // this only works for the base, which is weird because it is the opposite of line 123
                return {toReturn, [transformRotation,transformPosition,transformBase](Point p){ return (Point(multiplyMatrices3x3And3x1(Matrix(transformRotation), Matrix(p))) - Point(multiplyMatrices3x3And3x1(Matrix(transformRotation), Matrix(Point(transformBase))))) + transformPosition; }};
                // return toReturn;
            }
        }
    }
    // return {toReturn, [transformRotation,transformPosition](Point p){ return sequence(p)[transformRotation] + transformPosition; }};
}

int main(int argc, char** argv) {
    auto scanners = parseInput();
    // for (Point p : sequence({'x','y','z'})) {
    //     std::cout << "{" << (p.x < 0 ? "-" : "") << (char)abs(p.x) << "," << (p.y < 0 ? "-" : "") << (char)abs(p.y) << "," << (p.z < 0 ? "-" : "") << (char)abs(p.z) << "}" << ",";
    // }
    // std::sort(scanners[0].begin(), scanners[0].end());
    // std::sort(scanners[1].begin(), scanners[1].end());
    // std::cout << scanners[0][0] << " " << scanners[1][0];
    // int i = 20;
    // Matrix m = multiplyMatrices3x3And3x1(Matrix(rotationMatrices[i]).transpose(), multiplyMatrices3x3And3x1(Matrix(rotationMatrices[i]), Matrix({{1},{2},{3}})));
    // std::cout << m;
    auto common = inCommon(scanners[0], scanners[1]);
    std::vector<Point> pts;
    std::function<Point(Point)> transform;
    std::tie(pts, transform) = common;
    for (Point p : pts) {
        std::cout << p << std::endl;
    }
    std::cout << transform({553,889,-390});
    return 0;
}