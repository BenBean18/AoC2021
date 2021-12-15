#include <iostream>
#include <fstream>
#include <vector>
#include <functional>
#include <queue>
#include <set>
#include <map>
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

// the priority queue implementation is from https://www.redblobgames.com/pathfinding/a-star/implementation.html
template<typename T, typename priority_t>
struct PriorityQueue {
    typedef std::pair<priority_t, T> PQElement;
    std::priority_queue<PQElement, std::vector<PQElement>, std::greater<PQElement>> elements;

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

    std::pair<std::map<T,T>, std::map<T,double>> dijkstra(T start, T end, std::function<double(T,T)> costFunction) {
        PriorityQueue<T,double> frontier; // frontier queue that returns the lowest value
        frontier.put(start, 0);
        std::map<T,T> cameFrom;
        // cameFrom[start] = NULL;
        std::map<T,double> costSoFar;
        // costSoFar[start] = NULL;

        while (!frontier.empty()) {
            T current = frontier.get();
            if (current == end) {
                return {cameFrom, costSoFar};
            }
            for (T neighbor : this->neighbors(current)) {
                double newCost = costSoFar[current] + costFunction(current, neighbor);
                if (std::find_if(costSoFar.begin(), costSoFar.end(), [neighbor](std::pair<T,double> i){ return i.first == neighbor; }) == costSoFar.end() || newCost < costSoFar[neighbor]) {
                    costSoFar[neighbor] = newCost;
                    double priority = newCost;
                    frontier.put(neighbor, priority);
                    cameFrom[neighbor] = current;
                }
            }
        }
        return {cameFrom, costSoFar};
    }

};

Graph<std::pair<std::pair<int,int>,int>> parseInput() {
    Graph<std::pair<std::pair<int,int>,int>> graph;
    std::vector<std::string> strings = getStrings();
    for (int y = 0; y < strings.size(); y++) {
        for (int x = 0; x < strings[y].size(); x++) {
            try {
                graph.addEdge({{y-1,x-1},strings.at(y-1).at(x-1)-'0'}, {{y,x},strings.at(y).at(x)-'0'});
            } catch (...) {
                // out of bounds, do nothing
            }
            try {
                graph.addEdge({{y-1,x+1},strings.at(y-1).at(x+1)-'0'}, {{y,x},strings.at(y).at(x)-'0'});
            } catch (...) {
                // out of bounds, do nothing
            }
            try {
                graph.addEdge({{y+1,x-1},strings.at(y+1).at(x-1)-'0'}, {{y,x},strings.at(y).at(x)-'0'});
            } catch (...) {
                // out of bounds, do nothing
            }
            try {
                graph.addEdge({{y+1,x+1},strings.at(y+1).at(x+1)-'0'}, {{y,x},strings.at(y).at(x)-'0'});
            } catch (...) {
                // out of bounds, do nothing
            }
        }
    }
    return graph;
}

int main(int argc, char** argv) {
    auto graph = parseInput();
    std::vector<std::string> strings = getStrings();
    auto result = graph.dijkstra({{0,0},strings[0][0]-'0'},{{strings.size()-1,strings[0].size()-1},strings[strings.size()-1][strings[0].size()-1]-'0'}, [](auto i, auto i2){ return (double)(i2.first.second); });
    int riskScore = 0;
    for (auto i : result.second) { // iterate from last to start
        riskScore += i.second;
    }
    std::cout << riskScore << std::endl;
    return 0;
}