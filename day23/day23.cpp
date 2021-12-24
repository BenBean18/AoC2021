#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <functional>
#include <cassert>
#include <queue>

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

struct Amphipod {
    int hallwayPosition = -1; // 0 = left, 10 = right
    int room = -1; // 0 is top left, 3 is top right, 4 is bottom left, 7 is bottom right
    bool inRoom = false;
    bool inHallway = false;
    char type; // 'A', 'B', 'C', 'D'
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
            toReturn.push_back(Amphipod(0, -1, false, true, type));
            for (int p = 1; p <= 9; p+=2) {
                toReturn.push_back(Amphipod(p, -1, false, true, type));
            }
            toReturn.push_back(Amphipod(10, -1, false, true, type));
        }
        if (inHallway) {
            toReturn.push_back(Amphipod(-1, (int)(type-'A'), true, false, type));
            toReturn.push_back(Amphipod(-1, (int)(type-'A')+4, true, false, type));
        }
        return toReturn;
    }
    
    static int roomToHallwayPosition(int r) {
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
    }
};

struct State {
    std::vector<Amphipod> amphipods; // D1,D2,C1,C2,B1,B2,A1,A2
    /*
    #############
    #0123456789:#
    ###;#<#=#>###
      #?#@#A#B#
      #########
    */
    std::vector<std::pair<State, double>> neighbors() { // vector of <state, cost>
        std::vector<std::pair<State, double>> states;
        for (int a = 0; a < amphipods.size(); a++) {
            State state = *this;
            double cost;
            for (Amphipod b : amphipods[a].neighbors()) {
                for (Amphipod aa : amphipods) {
                    if (!b.collidingWith(amphipods[a], aa)) {
                        state.amphipods[a] = b;
                        cost = Amphipod::cost(amphipods[a], b);
                    }
                }
            }
            states.push_back({state, cost});
        }
        return states;
    }
    // do the dijkstra! (do do do do do do do doo doo, do do do do do do-do doo doo)
    // (on the states, using this function)
};

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

    std::pair<std::map<T,T>, std::map<T,double>> dijkstra(T start, T end, std::function<double(T,T)> costFunction, std::function<void(PriorityQueue<T,double>,T,T,std::map<T,T>,double,std::map<T,std::set<T>>)> debugVisualization = [](PriorityQueue<T,double> /**/,T /**/,T /**/,std::map<T,T> /**/,double /**/,std::map<T,std::set<T>> /**/){}) {
        PriorityQueue<T,double> frontier; // frontier queue that returns the lowest value
        frontier.put(start, 0);
        std::map<T,T> cameFrom;
        std::map<T,double> costSoFar;

        // This drastically improves speed (more than 10x), find() is slow
        std::map<T,bool> visited;

        while (!frontier.empty()) {
            T current = frontier.get();
            if (current == end) {
#ifdef VISUALIZE
                debugVisualization(frontier, current, current, cameFrom, 0, connections);
#endif
                return {cameFrom, costSoFar};
            }
            for (T neighbor : this->neighbors(current)) {
                double newCost = costSoFar[current] + costFunction(current, neighbor);
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

std::pair<std::map<char, char>, Graph<char>> parseInput() {
    std::vector<std::string> strings = getStrings();
    Graph<char> graph;
    std::map<char, char> graphMap;
    // hallway
    graph.addEdge('0','1');
    graph.addEdge('1','3');
    graph.addEdge('3','5');
    graph.addEdge('5','7');
    graph.addEdge('7','9');
    graph.addEdge('9',':');
    // rooms
    graph.addEdge('1',';');
    graph.addEdge('3',';');
    graph.addEdge('3','<');
    graph.addEdge('5','<');
    graph.addEdge('5','=');
    graph.addEdge('7','=');
    graph.addEdge('7','>');
    graph.addEdge('9','>');
    
    // second rooms
    graph.addEdge(';','?');
    graph.addEdge('<','@');
    graph.addEdge('=','A');
    graph.addEdge('>','B');

    char counter = '0';
    for (std::string s : strings) {
        for (char c : s) {
            if ((c != '#') && (c != ' ')) {
                graphMap[counter] = c;
                counter++;
            }
        }
    }
    assert(counter == 'C');
    return {graphMap, graph};
}

int main(int argc, char** argv) {
    Graph<char> graph;
    std::map<char, char> graphMap;
    std::tie(graphMap, graph) = parseInput();
    std::map<char, double> costMap{{'A',1},{'B',10},{'C',100},{'D',1000}};
    std::vector<char> As;
    std::vector<char> Bs;
    std::vector<char> Cs;
    std::vector<char> Ds;
    std::vector<char> agents;
    for (auto i : graphMap) {
        if (i.second == 'A') {
            As.push_back(i.first);
        }
        if (i.second == 'B') {
            Bs.push_back(i.first);
        }
        if (i.second == 'C') {
            Cs.push_back(i.first);
        }
        if (i.second == 'D') {
            Ds.push_back(i.first);
        }
    }
    for (char d : Ds) {
        agents.push_back(d);
    }
    for (char c : Cs) {
        agents.push_back(c);
    }
    for (char b : Bs) {
        agents.push_back(b);
    }
    for (char a : As) {
        agents.push_back(a);
    }
    for (char a : agents) {
        std::cout << a << " ";
    }
    std::function<double(char, char)> costFunction = [graphMap,costMap](char a, char b){ return 2 * (graphMap.at(a)>'@'?costMap.at(graphMap.at(a)):costMap.at(graphMap.at(b))); };
    return 0;
}