#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <algorithm>
#include <regex>
#include <set>

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
    int16_t x;
    int16_t y;
    int16_t z;
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
    long hash() {
        return ((unsigned long)this->x)*30UL+((unsigned long)this->y)*20UL+((unsigned long)this->z);
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
inline bool operator<(Point a, Point b) { // there was a problem with this and the greater than operator, there were not unique values
    return a.hash() < b.hash();
}
inline bool operator>(Point a, Point b) {
    return a.hash() < b.hash();
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
        const Matrix m = rotationMatrices[rot];
        for (int base = 0; base < points.size(); base++) {
            std::vector<Point> possibility; // there will be 600 of these
            for (int p = 0; p < points.size(); p++) {
                // auto pr = Point(multiplyMatrices3x3And3x1(m, Matrix(points[p])));
                // auto br = Point(multiplyMatrices3x3And3x1(m, Matrix(points[base])));
                // Point current = pr - br;
                Point current = Point(multiplyMatrices3x3And3x1(m, Matrix(points[p]-points[base])));
                possibility.push_back(current);
            }
            possibilities.push_back({possibility, Matrix(m), points[base]});
        }
    }
    return possibilities;
}

std::vector<Possibility> basePossibilities(std::vector<Point> points) { 
    std::vector<Possibility> possibilities; // 24 rotations * how many possible relative offsets
    for (auto &i : possibilities) {
        i.p.resize(points.size());
    }
    for (int rot = 0; rot < 1; rot++) {
        const Matrix m = rotationMatrices[rot];
        for (int base = 0; base < points.size(); base++) {
            std::vector<Point> possibility; // there will be 600 of these
            for (int p = 0; p < points.size(); p++) {
                // auto pr = Point(multiplyMatrices3x3And3x1(m, Matrix(points[p])));
                // auto br = Point(multiplyMatrices3x3And3x1(m, Matrix(points[base])));
                // Point current = pr - br;
                Point current = points[p] - points[base];
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
    auto sc1 = basePossibilities(scanner1);
    std::vector<Point> toReturn(scanner1.size());
    Matrix transformRotation;
    Matrix transformRotation1;
    Point transformPosition;
    Point transformBase;
    for (auto p1 : sc1) { // scanner 1 is all possibilities
        for (auto p2 : sc2) { // scanner 2 is all possibilities
            std::sort(p1.p.begin(), p1.p.end());
            std::sort(p2.p.begin(), p2.p.end());
            std::vector<Point> inCommon;
            auto it = std::set_intersection(p2.p.begin(), p2.p.end(), p1.p.begin(), p1.p.end(), std::back_inserter(inCommon));
            // inCommon.resize(it-inCommon.begin());
            if (inCommon.size() >= 12) {
                for (Point p : inCommon) {
                    assert(std::find(p2.p.begin(), p2.p.end(), p) != p2.p.end());
                    assert(std::find(p1.p.begin(), p1.p.end(), p) != p1.p.end());
                }
                for (auto &p : inCommon) {
                    p = p + p1.b;
                }
                toReturn = inCommon;
                transformRotation = p2.r; // p2.r is the rotation from scanner2 (relative to base) to the rotated scanner1
                transformRotation1 = p1.r; // p1.r is the rotation from scanner1 (relative to base) to the rotated scanner2
                transformPosition = p1.b;
                transformBase = p2.b;
                return {toReturn, [transformRotation,transformRotation1,transformPosition,transformBase](Point p){ return (Point(multiplyMatrices3x3And3x1(Matrix(transformRotation), Matrix(p))) - Point(multiplyMatrices3x3And3x1(Matrix(transformRotation), Matrix(Point(transformBase))))) + transformPosition; }};
                // return toReturn;
            }
        }
    }
    return {{}, std::function<Point(Point)>()};
}

template <typename T>
struct MemoryEfficientSet : public std::vector<T> {
    void insert(T el) {
        if (std::find(this->begin(), this->end(), el) == this->end()) {
            this->push_back(el);
        }
    }
};

int main(int argc, char** argv) {
    auto scanners = parseInput();
    MemoryEfficientSet<Point> uniquePoints;
    std::vector<std::function<Point(Point)>> transforms(scanners.size()); // transforms[scanner] is the function to transform that scanner to 0
    std::vector<bool> transformFound(scanners.size()); // if transformFound[scanner] is false, there is no transform yet
    int transformsFound = 0;
    std::vector<int> bIndices;
    for (int i = 0; i < scanners.size(); i++) {
        bIndices.push_back(i);
    }
    // account for the fact that 0 is the base transform
    transforms[0] = [](Point p){ return p; };
    bIndices.erase(bIndices.begin()+0);
    transformsFound++;
    transformFound[0] = true;
    for (Point p : scanners[0]) {
        uniquePoints.insert(p);
    }

    std::map<std::pair<int,int>,bool> noOverlap; // if noOverlap[{a,b}] is true, don't bother checking

    while (transformsFound < scanners.size()) {
        for (int a = 0; a < scanners.size(); a++) {
            if (!transformFound[a]) {
                continue;
            }
            bool matchFound = false;
            for (int b : bIndices) {
                if (a == b || noOverlap[{a,b}]) {
                    continue;
                }
                std::cout << "trying " << a << " -> " << b << "...";
                auto common = inCommon(scanners[a], scanners[b]);
                std::cout << "tried...";
                std::vector<Point> pts;
                std::function<Point(Point)> transform;
                std::tie(pts, transform) = common;
                if (pts.size() != 0) {
                    transforms[b] = [a,transform,transforms](Point p){ return transforms[a](transform(p)); };
                    for (Point p : scanners[b]) {
                        uniquePoints.insert(transforms[b](p));
                    }
                    transformFound[b] = true;
                    transformsFound += 1;
                    bIndices.erase(std::remove(bIndices.begin(), bIndices.end(), b), bIndices.end());
                    std::cout << "match" << std::endl;
                    matchFound = true;
                    break;
                } else {
                    noOverlap[{a,b}] = true;
                    std::cout << std::endl;
                }
            }
            if (matchFound) {
                break;
            }
        }
        for (int i = 0; i < transformFound.size(); i++) {
            if (!transformFound[i]) {
                std::cout << i << " ";
            }
        }
        std::cout << "-- " << uniquePoints.size() << " points so far" << std::endl;
    }
    return 0;
}