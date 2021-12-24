#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <functional>
#include <cassert>
#include <queue>
#include <algorithm>

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

unsigned long long hashStr(const char* s, unsigned long long salt)
{
    unsigned long long h = salt;
    while (*s)
        h = h * 101 + (unsigned long long) *s++;
    return h;
}

struct Amphipod {
    int8_t hallwayPosition = -1; // 0 = left, 10 = right
    int8_t room = -1; // 0 is top left, 3 is top right, 4 is bottom left, 7 is bottom right
    bool inRoom = false;
    bool inHallway = false;
    char type; // 'A', 'B', 'C', 'D'
    Amphipod() {

    }
    Amphipod(int h, int r, bool inH, bool inR, char t) {
        hallwayPosition = h;
        room = r;
        inHallway = inH;
        inRoom = inR;
        type = t;
    }
    std::vector<Amphipod> neighbors() {
        std::vector<Amphipod> toReturn;
        if (inRoom) {
            toReturn.push_back(Amphipod(0, -1, true, false, type));
            for (int p = 1; p <= 9; p+=2) {
                toReturn.push_back(Amphipod(p, -1, true, false, type));
            }
            toReturn.push_back(Amphipod(10, -1, true, false, type));
        }
        if (inHallway) {
            toReturn.push_back(Amphipod(-1, (int)(type-'A'), false, true, type));
            toReturn.push_back(Amphipod(-1, (int)(type-'A')+4, false, true, type));
        }
        return toReturn;
    }
    
    static int8_t roomToHallwayPosition(int8_t r) {
        return 2+((r % 4)*2);
    }

    bool collidingWith(Amphipod initial, Amphipod b) {
        if (inRoom && b.inRoom) {
            if (room == b.room) {
                return true;
            }
        }
        if (inHallway && b.inHallway) {
            if (hallwayPosition == b.hallwayPosition) {
                return true;
            }
            if (roomToHallwayPosition(initial.room) <= b.hallwayPosition && b.hallwayPosition <= hallwayPosition) {
                return true;
            }
            if (hallwayPosition <= b.hallwayPosition && b.hallwayPosition <= roomToHallwayPosition(initial.room)) {
                return true;
            }
        }
        return false;
    }

    static double cost(Amphipod a, Amphipod b) {
        assert((a.inRoom && b.inHallway) || (b.inRoom && a.inHallway));
        if (a.inRoom) {
            return (std::max(b.hallwayPosition, roomToHallwayPosition(a.room)) - std::min(b.hallwayPosition, roomToHallwayPosition(a.room))) + (a.room/4);
        }
        if (b.inRoom) {
            return (std::max(a.hallwayPosition, roomToHallwayPosition(b.room)) - std::min(a.hallwayPosition, roomToHallwayPosition(b.room))) + (b.room/4);
        }
        std::cout << "rut row!" << std::endl;
    }

    void printMe() {
        std::cout << "Hash " << this->hash() << ". Type " << type << ". " << (inHallway?((std::string)"Hallway: "+std::to_string(hallwayPosition)):"") << (inRoom?((std::string)"Room: "+std::to_string(room)):"") << std::endl;
    }

    std::string strHash() {
        std::string s{(char)(hallwayPosition + 40), (char)((room % 4) + 50), (char)(inRoom + 60), (char)(inHallway + 70), type};
        return s;
    }

    unsigned long hash() {
        const char s[] = {(char)(hallwayPosition + 40), (char)((room % 4) + 50), (char)(inRoom + 60), (char)(inHallway + 70), type};
        return hashStr(s, 512);
    }
};
inline bool operator<(Amphipod a, Amphipod b) {
    return a.hash() < b.hash();
}
inline bool operator==(Amphipod a, Amphipod b) {
    return a.hash() == b.hash();
}

struct State {
    std::vector<Amphipod> amphipods; // D1,D2,C1,C2,B1,B2,A1,A2
    /*
    #############
    #0123456789:#
    ###;#<#=#>###
      #?#@#A#B#
      #########
    */
    State(std::vector<Amphipod> amphipods) {
        this->amphipods = amphipods;
    }
    State() {

    }
    static std::vector<std::pair<State, double>> neighbors(State s) { // vector of <state, cost>
        std::vector<std::pair<State, double>> states;
        for (int a = 0; a < s.amphipods.size(); a++) {
            State state(s);
            double cost;
            for (Amphipod b : s.amphipods[a].neighbors()) {
                bool collision = false;
                for (int aa = 0; aa < s.amphipods.size(); aa++) {
                    if (aa == a) {
                        continue;
                    }
                    if (b.collidingWith(s.amphipods[a], s.amphipods[aa])) {
                        collision = true;
                    }
                }
                if (!collision) {
                    state.amphipods[a] = b;
                    cost = Amphipod::cost(s.amphipods[a], b);
                    states.push_back({state, cost});
                    state = State(s);
                }
            }
            // std::cout << std::endl;
        }
        return states;
    }
    unsigned long long hash() {
        std::string data;
        std::vector<Amphipod> as(amphipods);
        std::sort(as.begin(), as.end());
        for (Amphipod a : as) {
            const char chars[] = {(char)(a.hallwayPosition + 40), (char)((a.room % 4) + 50), (char)(a.inRoom + 60), (char)(a.inHallway + 70), a.type};
            data.append(chars);
        }
        return hashStr(data.c_str(), 123);
    }
};
inline bool operator==(State a, State b) {
    // std::vector<Amphipod> intersection(8);
    // std::sort(a.amphipods.begin(), a.amphipods.end());
    // std::sort(b.amphipods.begin(), b.amphipods.end());
    // intersection.resize(std::set_intersection(a.amphipods.begin(), a.amphipods.end(), b.amphipods.begin(), b.amphipods.end(), intersection.begin()) - intersection.begin());
    // return intersection.size() == 8;
    return a.hash() == b.hash();
}
inline bool operator<(State a, State b) {
    // bool lessThan = true;
    // std::sort(a.amphipods.begin(), a.amphipods.end());
    // std::sort(b.amphipods.begin(), b.amphipods.end());
    // assert(a.amphipods.size() == 8 && b.amphipods.size() == 8);
    // for (int i = 0; i < 8; i++) {
    //     lessThan = lessThan && (a.amphipods[i] < b.amphipods[i]);
    // }
    // return lessThan;
    return a.hash() < b.hash();
}

// Notes:
// Maybe try A* on all amphipods in order. If an amphipod can't move that cycle, it passes and no move is made.
// Probably prioritize D, then C, then B, then A (order of energy use)
// D1, D2, C1, C2, B1, B2, A1, A2 each do A* every cycle, then do it again.

// While all pairs have not been moved:
// 1. Check which pair is closest and what is blocking

// For each move by each agent, the cost should depend on the movement costs for all other agents (with more energy having a higher cost) so each move has lowest energy for all

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

// Graph map: key: (see graph values below), value: '.' = open, 'A' = amber, 'B' = bronze, 'C' = copper, 'D' = desert
// Graph values (ascii, starting at '0'):
/*
#############
#0123456789:#
###;#<#=#>###
  #?#@#A#B#
  #########
*/

// Inspiration from reddit:
/*
#############        2 2 2 2
#...........#   == ..A.B.C.D..
###A#B#C#D###
  #A#B#C#D#
  #########
*/
// Map can be represented as:
// 0123456789

State parseInput() {
    std::vector<std::string> strings = getStrings();
    std::vector<Amphipod> amphipods;
    for (int i = 2; i < strings.size(); i++) { // skip first row, it's empty
        for (int j = 0; j < strings[i].size(); j++) {
            if (strings[i][j] >= 'A' && strings[i][j] <= 'D') { // 3, 5, 7, or 9. -3 = 0,2,4,or 6. /2 = 0,1,2,3
                amphipods.push_back(Amphipod(-1, (j-3)/2 + (i-2)*4, false, true, strings[i][j]));
            }
        }
    }
    return State(amphipods);
}

int main(int argc, char** argv) {
    State state = parseInput();
    State end{{Amphipod(-1,0,false,true,'A'),Amphipod(-1,4,false,true,'A'),Amphipod(-1,1,false,true,'B'),Amphipod(-1,5,false,true,'B'),Amphipod(-1,2,false,true,'C'),Amphipod(-1,6,false,true,'C'),Amphipod(-1,3,false,true,'D'),Amphipod(-1,7,false,true,'D')}};
    Graph<State> graph;
    std::cout << graph.dijkstra(state, end, State::neighbors).second[end] << std::endl;
    return 0;
}