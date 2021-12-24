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

#define PART2

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
    // This is 4 rooms, but std::array was being weird
    std::vector<std::vector<char>> rooms; // char is 'A','B','C','D'
    bool pathIsClear(int room, int hallwayPos) {
        if (hallwayPos > room+2) {
            for (int i = room+2; i < hallwayPos; i++) {
                if (hallway[i] != '.') {
                    return false;
                }
            }
            return true;
        } else {
            for (int i = hallwayPos; i < room+2; i++) {
                if (hallway[i] != '.') {
                    return false;
                }
            }
            return true;
        }
        return false;
    }
    static std::vector<std::pair<State, double>> neighbors(State s) { // returns vector{{state, additionalCost}}
        std::vector<std::pair<State, double>> states;
        for (int i = 0; i < s.rooms.size(); i++) { // room -> hallway
            auto r = s.rooms[i];
            if (r.size() > 0) {
                char amphipod = r[r.size()-1];
                State copy(s);
                copy.rooms[i].pop_back();
                for (int j = 0; j < s.hallway.size(); j++) {
                    State newState(copy);
                    if (s.hallway[j] == '.' && s.pathIsClear(i, j)) {
                        newState.rooms[i].pop_back();
                        newState.hallway[j] = amphipod;
                    }
                    //                          distance up         + dist in hall
                    std::pair<State, double> p;
                    p.first.hallway = newState.hallway;
                    for (int i = 0; i < 4; i++) {
                        for (char c : newState.rooms[i]) {
                            p.first.rooms[i].push_back(c);
                        }
                    }
                    p.second = (LENGTH-r.size())+1 + fabs(j - (i+1.5))+0.5;
                    states.push_back(p);
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
                if (roomIsGood && s.pathIsClear(v-'A', i)) {
                    State newState(s);
                    newState.hallway[i] = '.';
                    newState.rooms[v-'A'].push_back(v);
                    std::pair<State, double> p;
                    p.first.hallway = newState.hallway;
                    for (int i = 0; i < 4; i++) {
                        for (char c : newState.rooms[i]) {
                            p.first.rooms[i].push_back(c);
                        }
                    }
                    p.second = (LENGTH-s.rooms[v-'A'].size()) + fabs(i - (v-'A'+1.5))+0.5;
                    states.push_back(p);
                }
            }
        }
        return states;
    }
    unsigned long long hash() {
        std::string h = hallway;
        for (auto r : rooms) {
            for (char c : r) {
                h.push_back(c);
            }
        }
        return hashStr(h.c_str(), 12232021);
    }
};
inline bool operator<(State a, State b) {
    return a.hash() < b.hash();
}
inline bool operator==(State a, State b) {
    return a.hash() == b.hash();
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

template<typename T>
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

    std::pair<std::map<T,T>, std::map<T,double>> dijkstra(T start, T end, std::function<std::vector<std::pair<T,double>>(T)> neighborCostFunction, std::function<void(PriorityQueue<T,double>,T,T,std::map<T,T>,double,std::map<T,std::set<T>>)> debugVisualization = [](PriorityQueue<T,double> /**/,T /**/,T /**/,std::map<T,T> /**/,double /**/,std::map<T,std::set<T>> /**/){}) {
        PriorityQueue<T,double> frontier; // frontier queue that returns the lowest value
        frontier.put(start, 0);
        std::map<T,T> cameFrom;
        std::map<T,double> costSoFar;

        std::cout << "dijkstra" << std::endl;

        // This drastically improves speed (more than 10x), find() is slow
        std::map<T,bool> visited;

        while (!frontier.empty()) {
            T current = frontier.get();
            if (current == end) {
#ifdef VISUALIZE
                debugVisualization(frontier, current, current, cameFrom, 0, connections);
#endif
                std::cout << "equal" << std::endl;
                return {cameFrom, costSoFar};
            }
            auto neighbors = neighborCostFunction(current);
            for (auto neighborCost : neighbors) {
                T neighbor = neighborCost.first;
                double newCost = costSoFar[current] + neighborCost.second;
                if (newCost > 15) {
                    std::cout << newCost << std::endl;
                }
                if (!visited[neighbor] || (newCost < costSoFar[neighbor])) {
                    visited[neighbor] = true;
                    costSoFar[neighbor] = newCost;
                    double priority = newCost;
                    frontier.put(neighbor, priority);
                    cameFrom[neighbor] = current;
#ifdef VISUALIZE
                    debugVisualization(frontier, current, neighbor, cameFrom, priority, connections);
#endif
                }
            }
        }
        return {cameFrom, costSoFar};
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
    std::vector<std::vector<char>> rooms;
    for (int _ = 2; _ < 2+LENGTH; _++) {
        for (int i = 3; i < 10; i+=2) {
            rooms[(i-3)/2].push_back(strings[_][i]);
        }
    }
    State s = State();
    s.rooms = rooms;
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
    Graph<State> graph;
    std::cout << graph.dijkstra(start, end, State::neighbors).second[end] << std::endl;
    return 0;
}