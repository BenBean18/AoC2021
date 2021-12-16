#include <iostream>
#include <fstream>
#include <vector>
#include <functional>
#include <queue>
#include <set>
#include <map>
#include <algorithm>
#include <chrono>
#include <thread>

#define BIG_MAP false
#define VISUALIZE // comment out to not visualize

void delay(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
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

std::vector<std::string> bigMap() {
    std::vector<std::string> strings = getStrings();
    int originalXSize = strings[0].size();
    int originalYSize = strings.size();
    for (int i = 1; i < 5; i++) {
        for (int y = 0; y < strings.size(); y++) {
            for (int x = 0; x < originalXSize; x++) {
                strings[y].push_back((strings[y][x] + i - '1') % 9 + '1');
            }
        }
    }
    for (int i = 1; i < 5; i++) {
        for (int y = 0; y < originalYSize; y++) {
            std::string line;
            for (int x = 0; x < strings[0].size(); x++) {
                line.push_back((strings[y][x] + i - '1') % 9 + '1');
            }
            strings.push_back(line);
        }
    }
    return strings;
}

std::vector<std::string> getMap() {
    if (BIG_MAP) {
        return bigMap();
    } else {
        return getStrings();
    }
}

Graph<std::pair<std::pair<int,int>,int>> parseInput() {
    Graph<std::pair<std::pair<int,int>,int>> graph;
    std::vector<std::string> strings = getMap();
    for (int y = 0; y < strings.size(); y++) {
        for (int x = 0; x < strings[y].size(); x++) {
            try {
                graph.addEdge({{y-1,x},strings.at(y-1).at(x)-'0'}, {{y,x},strings.at(y).at(x)-'0'});
            } catch (...) {
                // out of bounds, do nothing
            }
            try {
                graph.addEdge({{y+1,x},strings.at(y+1).at(x)-'0'}, {{y,x},strings.at(y).at(x)-'0'});
            } catch (...) {
                // out of bounds, do nothing
            }
            try {
                graph.addEdge({{y,x-1},strings.at(y).at(x-1)-'0'}, {{y,x},strings.at(y).at(x)-'0'});
            } catch (...) {
                // out of bounds, do nothing
            }
            try {
                graph.addEdge({{y,x+1},strings.at(y).at(x+1)-'0'}, {{y,x},strings.at(y).at(x)-'0'});
            } catch (...) {
                // out of bounds, do nothing
            }
        }
    }
    return graph;
}

typedef std::pair<std::pair<int,int>,int> N;
void debugVisualization(PriorityQueue<N,double> frontier, N current, N next, std::map<N,N> cameFrom, double cost, std::map<N,std::set<N>> connections) {
    auto m = getMap();
    std::vector<std::vector<int>> map(m.size());
    for (int i = 0; i < m.size(); i++) {
        map[i].resize(m[0].size());
    }
    for (auto c : connections) {
        map[c.first.first.first][c.first.first.second] = c.first.second;
    }
    std::vector<N> orderedFrontier; // ordered by priority, most prioritized at index 0
    for (int _ = 0; _ < frontier.elements.size(); _++) {
        orderedFrontier.push_back(frontier.get());
    }
    std::pair<int,int> currentYX = current.first;
    std::pair<int,int> nextYX = next.first;
    std::cout << "\033[H"; // cursor to top left
    int frontierColor = 5;
    for (int y = 0; y < map.size(); y++) {
        for (int x = 0; x < map[0].size(); x++) {
            N thisNode = {{y,x},map[y][x]};
            std::vector<N> thisPath;
            N currentNode = current;
            while (!(currentNode.first.first == 0 && currentNode.first.second == 0)) {
                thisPath.push_back(currentNode);
                currentNode = cameFrom[currentNode];
            }
            thisPath.push_back({{0,0},map[0][0]});
            if (currentYX.first == y && currentYX.second == x) {
                std::cout << "\033[48;5;226m";
                std::cout << map[y][x];
                std::cout << "\033[0m";
            } else if (nextYX.first == y && nextYX.second == x) {
                std::cout << "\033[48;5;120m";
                std::cout << map[y][x];
                std::cout << "\033[0m";
            }
            else if (std::find(orderedFrontier.begin(), orderedFrontier.end(), thisNode) != orderedFrontier.end()) {
                frontierColor += (int)(255 / orderedFrontier.size());
                std::cout << "\033[48;2;0;" << frontierColor << ";" << frontierColor << "m";
                std::cout << map[y][x];
                std::cout << "\033[0m";
            } else if (std::find(thisPath.begin(), thisPath.end(), thisNode) != thisPath.end()) {
                std::cout << "\033[48;5;207m";
                std::cout << map[y][x];
                std::cout << "\033[0m";
            }
            else {
                std::cout << map[y][x];
            }
        }
        std::cout << std::endl;
    }
    std::flush(std::cout);
    if (!BIG_MAP) {
        delay(100);
    }
}

int main(int argc, char** argv) {
    auto graph = parseInput();
    std::vector<std::string> strings = getMap();
    std::cout << "done parsing input" << std::endl;
    std::cout << "\033[H\033[J"; // cursor to top left and clear screen
    auto result = graph.dijkstra({{0,0},strings[0][0]-'0'},{{strings.size()-1,strings[0].size()-1},strings[strings.size()-1][strings[0].size()-1]-'0'}, [](auto i, auto i2){ return (double)(i2.second); }, debugVisualization); // fixed score function bug
#ifdef VISUALIZE
    delay(1000); // If we're visualizing, pause so we can see it
#endif
    int riskScore = 0;
    std::pair<std::pair<int,int>,int> current = {{strings.size()-1,strings[0].size()-1},strings[strings.size()-1][strings[0].size()-1]-'0'};
    std::pair<std::pair<int,int>,int> start = {{0,0},strings[0][0]-'0'};
    std::vector<std::pair<std::pair<int,int>,int>> path;
    std::cout << "done with dijkstra" << std::endl;
    std::flush(std::cout);
    std::vector<std::pair<std::pair<int,int>,int>> visited = {current};
    while (!(current.first.first == 0 && current.first.second == 0)) {
        path.push_back(current);
        current = result.first[current];
    }
    path.push_back(start);
    for (int i = path.size()-1; i >= 0; i--) {
        riskScore += strings[path[i].first.first][path[i].first.second] - '0';
        std::cout << path[i].first.second << "," << path[i].first.first << std::endl;
    }
    riskScore -= strings[0][0] - '0';
    std::cout << "risk: " << riskScore << std::endl;
    return 0;
}