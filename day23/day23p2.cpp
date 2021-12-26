#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <array>
#include <map>
#include <set>
#include <functional>
#include <cassert>
#include <queue>
#include <algorithm>
#include <bitset>

#define PART1

#ifdef PART2
#define LENGTH 4
#else
#define LENGTH 2
#endif

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

//                0 1 2 3
// Represent as ..R.R.R.R.. where R is vector<char>
//              01 2 3 4 56
// Room to hallway: room+1.5

unsigned long long hashStr(const char* s, unsigned long long salt)
{
    unsigned long long h = salt;
    while (*s)
        h = h * 101 + (unsigned long long) *s++;
    return h;
}

struct State {
    std::string hallway = ".......";
    std::array<std::vector<char>,4> rooms; // char is 'A','B','C','D'
    State() {
        rooms.fill({});
    }
    bool pathIsClear(int room, int hallwayPos, bool debug = false) {
        // room + 1 is left, room + 2 is right
        if (hallwayPos >= room+2) {
            if (debug) {
                std::cout << "\t-";
            }
            for (int i = room+2; i < hallwayPos; i++) {
                if (debug) {
                    std::cout << i << "=" << hallway[i];
                }
                if (hallway[i] != '.') {
                    return false;
                }
            }
            if (debug) {
                std::cout << "-\t";
            }
            return true;
        } else {
            if (debug) {
                std::cout << "\t-";
            }
            for (int i = hallwayPos+1; i < room+2; i++) {
                if (debug) {
                    std::cout << i << "=" << hallway[i];
                }
                if (hallway[i] != '.') {
                    return false;
                }
            }
            if (debug) {
                std::cout << "-\t";
            }
            return true;
        }
        return false;
    }
    static int extend(int pos) {
        // std::string extendedHallway = ".. . . . ..";
        std::map<int,int> extendedHallwayPos = {{0,0},{1,1},{2,3},{3,5},{4,7},{5,9},{6,10}};
        return extendedHallwayPos[pos];
    }
    static int extendRoom(int room) {
        // std::string extendedHallway = "..R.R.R.R..";
        std::map<int,int> extendedHallwayPos = {{0,2},{1,4},{2,6},{3,8}};
        return extendedHallwayPos[room];
    }
    static int actualDist(int /*hall*/pos1, int /*room*/pos2) {
        return extend(std::max(pos1,pos2)) - extend(std::min(pos1,pos2));
        // return abs(extendRoom(room) - extend(hall));
    }
    static double horizDistance(int room, int hallwayPos) {
        if (hallwayPos >= room+2) {
            return actualDist(hallwayPos, room+2) + 1;
        } else {
            return actualDist(hallwayPos, room+1) + 1;
        }
        // return actualDist(hallwayPos, room);
    }
    static std::vector<std::pair<State, double>> neighbors(State s) { // returns vector{{state, additionalCost}}
        std::vector<std::pair<State, double>> states{};
        for (int i = 0; i < s.rooms.size(); i++) { // room -> hallway
            auto r = s.rooms[i];
            if (r.size() > 0) {
                char amphipod = r[r.size()-1];
                bool roomCorrect = true;
                for (char c : r) {
                    if (c != i+'A') {
                        roomCorrect = false;
                    }
                }
                if (roomCorrect) { // if both in destination room, pass
                    continue;
                }
                State copy(s);
                for (int j = 0; j < s.hallway.size(); j++) {
                    // std::cout << "In state" << std::endl;
                    // s.printMe();
                    // std::cout << "Path between room " << i << " and position " << j << (s.pathIsClear(i, j, true)?" is ":" isn't ") << "clear." << std::endl;
                    if (s.hallway[j] == '.' && s.pathIsClear(i, j)) {
                        // std::cout << "path is clear between room " << i << " and hallway " << j << std::endl << "state: ";
                        State newState((const State)copy);
                        newState.rooms[i].pop_back();
                        newState.hallway[j] = amphipod;
                        // newState.printMe();
                        //                          distance up          + dist in hall
                        states.push_back({newState, ((LENGTH-r.size())+1 + horizDistance(i, j))*pow(10, amphipod-'A')});
                    } else {
                        // std::cout << "path is not clear between room " << i << " and hallway " << j << std::endl << "state: ";
                    }
                }
            }
        }
        for (int i = 0; i < s.hallway.size(); i++) { // hallway -> room
            char v = s.hallway[i];
            if (v != '.') {
                bool roomIsGood = false;
                if (s.rooms[v-'A'].size() < 2) {
                    roomIsGood = true;
                    for (char c : s.rooms[v-'A']) {
                        if (c != v) {
                            roomIsGood = false;
                        }
                    }
                }
                // if (roomIsGood) {
                //     std::cout << "In state" << std::endl;
                //     s.printMe();
                //     std::cout << "Room " << v << " is clear, containing ";
                //     for (char c : s.rooms[v-'A']) {
                //         std::cout << c;
                //     }
                //     std::cout << ". Path between room " << v-'A' << " and position " << i << (s.pathIsClear(v-'A', i, true)?" is ":" isn't ") << "clear." << std::endl;
                // }
                if (roomIsGood && s.pathIsClear(v-'A', i)) {
                    State newState(s);
                    newState.hallway[i] = '.';
                    newState.rooms[v-'A'].push_back(v);
                    //                          distance up                     + dist in hall
                    states.push_back({newState, ((LENGTH-s.rooms[v-'A'].size()) + horizDistance(v-'A', i))*pow(10,v-'A')});
                }
            }
        }
        return states;
    }
    void printMe() {
        // std::string newHallway = "";
        // newHallway = newHallway + hallway[0] + hallway[1] + "." + hallway[2] + "." + hallway[3] + "." + hallway[4] + "." + hallway[5] + hallway[6];
        std::cout << hallway << std::endl;
        for (int i = 0; i < rooms.size(); i++) {
            auto r = rooms[i];
            for (char c : r) {
                std::cout << c;
            }
            std::cout << "-";
        }
        std::cout << std::endl;
    }
    static unsigned int charToBase5(char c) {
        return (c=='.'?0:((c-'A')+1));
    }
    unsigned long long hash() {
        std::string h = hallway;
        unsigned char h2[8];
        for (auto r : rooms) {
            for (int i = 0; i < LENGTH; i++) {
                try {
                    h.push_back(r.at(i));
                } catch (...) {
                    h.push_back('.');
                }
            }
        }
        for (int i = 3; i > h.size() % 3; i--) {
            h.push_back('.');
        }
        // for (int i = 0; i < h.size(); i+=3) {
        //     h2[i/3] = charToBase5(h[i])*25+charToBase5(h[i+1])*5+charToBase5(h[i+2]);
        // }
        // unsigned long long hash = 0;
        // hash += (unsigned long long)h2[0] << 56;
        // hash += (unsigned long long)h2[1] << 48;
        // hash += (unsigned long long)h2[2] << 40;
        // hash += (unsigned long long)h2[3] << 32;
        // hash += (unsigned long long)h2[4] << 24;
        // hash += (unsigned long long)h2[5] << 16;
        // hash += (unsigned long long)h2[6] << 8;
        // hash += (unsigned long long)h2[7];
        // return hash;
        return hashStr(h.c_str(), 123);
        // return std::hash<std::string>(h2);
    }
};
inline bool operator<(State a, State b) {
    return a.hash() < b.hash();
}
inline bool operator==(State a, State b) {
    return a.hash() == b.hash();
}
inline bool operator!=(State a, State b) {
    return a.hash() != b.hash();
}

// Copied from Day 15
// from https://stackoverflow.com/questions/16749723/how-i-can-find-value-in-priority-queue
template<
    class T,
    class Container = std::vector<T>,
    class Compare = std::less<typename Container::value_type>
> class PublicPQ : public std::priority_queue<T, Container, Compare>
{
public:
    typedef typename
        std::priority_queue<
        T,
        Container,
        Compare>::container_type::const_iterator const_iterator;
    
    std::vector<T> c;
};

// the priority queue implementation is from https://www.redblobgames.com/pathfinding/a-star/implementation.html
template<typename T, typename priority_t>
struct PriorityQueue {
    typedef std::pair<priority_t, T> PQElement;
    PublicPQ<PQElement, std::vector<PQElement>, std::greater<PQElement>> elements;

    inline bool empty() const {
        return elements.empty();
    }

    inline void put(T item, priority_t priority) {
        elements.emplace(priority, item);
    }

    T get() {
        T best_item = elements.top().second;
        elements.pop();
        return best_item;
  }
};

std::map<unsigned long long,std::vector<std::pair<State, double>>> memo; // key = State::hash
double printThreshold = 0;
bool printNext = true;

using T = State;
class Graph {
    std::map<T,std::set<T>> connections;
public:
    void addEdge(T node1, T node2) {
        connections[node1].insert(node2);
        connections[node2].insert(node1);
    }

    std::set<T> neighbors(T node) {
        return connections[node];
    }

    std::tuple<std::map<T,double>, std::map<T,T>> dijkstra(T start, T end, std::function<std::vector<std::pair<T,double>>(T)> neighborCostFunction, std::function<void(PriorityQueue<T,double>,T,T,std::map<T,T>,double,std::map<T,std::set<T>>)> debugVisualization = [](PriorityQueue<T,double> /**/,T /**/,T /**/,std::map<T,T> /**/,double /**/,std::map<T,std::set<T>> /**/){}) {
        // only returns cost
        PriorityQueue<T,double> frontier; // frontier queue that returns the lowest value
        frontier.put(start, 0);
        std::map<T,T> cameFrom; // key = State::hash
        std::map<T,double> costSoFar; // key = State::hash

        std::cout << "dijkstra" << std::endl;

        // // This drastically improves speed (more than 10x), find() is slow
        // std::map<T,bool> visited; // key = State::hash

        while (!frontier.empty()) {
            T current = frontier.get();
            if (current == end) {
#ifdef VISUALIZE
                debugVisualization(frontier, current, current, cameFrom, 0, connections);
#endif
                std::cout << "equal" << std::endl;
                return {costSoFar, cameFrom};
            }
            std::vector<std::pair<State, double>> neighbors;
            try {
                neighbors = memo.at(current.hash());
            } catch (...) {
                neighbors = neighborCostFunction(current);
            }
            // auto neighbors = neighborCostFunction(current);
            current.printMe();
            for (auto neighborCost : neighbors) {
                T neighbor = neighborCost.first;
                double newCost = costSoFar[current] + neighborCost.second;
                if ((costSoFar[neighbor] == 0) || (newCost < costSoFar[neighbor])) {
                    neighbor.printMe();
                    std::cout << newCost << std::endl;
                    // if (newCost > printThreshold) {
                    //     std::cout << newCost << "(added " << neighborCost.second << ")" << std::endl;
                    //     current.printMe();
                    //     neighbor.printMe();
                    //     printThreshold = newCost + 10;
                    // }
                    // visited[neighbor] = true;
                    costSoFar[neighbor] = newCost;
                    double priority = newCost;
                    frontier.put(neighbor, priority);
                    cameFrom[neighbor] = current;
                    if (neighbor.hash() == end.hash()) {
                        std::cout << "Found result! " << newCost << std::endl;
                    }
#ifdef VISUALIZE
                    debugVisualization(frontier, current, neighbor, cameFrom, priority, connections);
#endif
                }
            }
            std::cout << std::endl;
        }
        return {costSoFar, cameFrom};
    }

};

State parseInput() {
    std::vector<std::string> strings = getStrings();
#ifdef PART2
    std::string toInsert1 = "  #D#C#B#A#";
    std::string toInsert2 = "  #D#B#A#C#";
    strings.insert(strings.begin() + 3, toInsert2);
    strings.insert(strings.begin() + 3, toInsert1);
#endif
    std::array<std::vector<char>,4> rooms;
    for (int _ = 2; _ < 2+LENGTH; _++) {
        for (int i = 3; i < 10; i+=2) {
            if (strings[_][i] != '.') {
                rooms[(i-3)/2].push_back(strings[_][i]);
            }
        }
    }
    for (int i = 0; i < 4; i++) {
        std::reverse(rooms[i].begin(), rooms[i].end());
    }
    std::string hallway = "";
    hallway = hallway + strings[1][1] + strings[1][2] + strings[1][4] + strings[1][6] + strings[1][8] + strings[1][10] + strings[1][11];
    State s = State();
    s.rooms = rooms;
    s.hallway = hallway;
    return s;
}

int main(int argc, char** argv) {
    State start = parseInput();
    State end = State();
    for (int _ = 0; _ < LENGTH; _++) {
        for (int room = 0; room < 4; room++) {
            end.rooms[room].push_back(room+'A');
        }
    }
    // for (auto n : State::neighbors(start)) {
    //     n.first.printMe();
    //     std::cout << n.second << std::endl;
    // }

    Graph graph;
    std::map<State, double> costSoFar;
    std::map<State, State> cameFrom;
    std::tie(costSoFar, cameFrom) = graph.dijkstra(start, end, State::neighbors);
    State current = end;
    while (current != start) {
        current.printMe();
        std::cout << costSoFar[current] << std::endl;
        current = cameFrom[current];
    }
    start.printMe();
    std::cout << costSoFar[end] << std::endl;
    return 0;
}