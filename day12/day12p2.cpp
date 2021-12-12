#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <regex>
#include <thread>
#include <mutex>

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

};

Graph<std::string> parseInput() {
    Graph<std::string> graph;
    std::vector<std::string> strings = getStrings();
    std::regex map("(.+)-(.+)");
    for (std::string s : strings) {
        std::smatch match;
        std::regex_match(s, match, map);
        if (match.size() == 3) {
            graph.addEdge(match[1], match[2]);
        }
    }
    return graph;
}

// pathFunction(end, vector<T> currentPath={start}, vector<T> &paths):
//     If currentPath[first] == start and currentPath[last] == end,
//         Add it to paths
//     For each neighbor of the last item in the path, 
//         call pathFunction, where currentPath is currentPath + neighbor

// Don't visit if already in the path and is a small room
// Once paths is computed, check to see the count of how many large vs small

static inline bool is_not_alphanum_lower(char c) {
    return (!isalnum(c) || !islower(c));
}

std::mutex mtx;
std::vector<std::thread> threads;

// currentPath is pair<path, hasVisitedASingleCaveTwiceYet>
void computePath(std::pair<std::vector<std::string>,bool> currentPath, Graph<std::string> graph, std::vector<std::vector<std::string>> &paths) {
    if (currentPath.first[currentPath.first.size()-1] == "end") {
        mtx.lock();
        paths.push_back(currentPath.first);
        mtx.unlock();
        return;
    }
    for (std::string node : graph.neighbors(currentPath.first[currentPath.first.size()-1])) {
        // if it's already in the list, it can't be a small cave (lowercase) because that would cause it to be visited multiple times
        // except if no small cave has been visited twice yet
        bool isSmallCave = node[0] >= 'a' && node[0] <= 'z';
        bool hasVisited = (std::find(currentPath.first.begin(), currentPath.first.end(), node) != currentPath.first.end());
        bool hasVisitedAndSmallCave = (hasVisited && isSmallCave);
        if (node == "end") {
            auto newPath = currentPath;
            newPath.first.push_back(node);
            mtx.lock();
            paths.push_back(currentPath.first);
            mtx.unlock();
        } else if (node == "start") {
            //
        } else if (!hasVisitedAndSmallCave) {
            auto newPath = currentPath;
            newPath.first.push_back(node);
            if (currentPath.first.size() == 1) {
                threads.emplace_back(computePath, newPath, graph, std::ref(paths));
            } else {
                computePath(newPath, graph, std::ref(paths));
            }
        } else if (hasVisitedAndSmallCave && (currentPath.second == false)) {
            auto newPath = currentPath;
            newPath.second = true; // this used to be currentPath.second = true, which was causing a bug...
            newPath.first.push_back(node);
            if (currentPath.first.size() == 1) {
                threads.emplace_back(computePath, newPath, graph, std::ref(paths));
            } else {
                computePath(newPath, graph, std::ref(paths));
            }
        }
    }
}

int main(int argc, char** argv) {
    auto graph = parseInput();
    std::vector<std::vector<std::string>> paths;
    computePath({{"start"}, false}, graph, paths);
    for (auto p : paths) {
        for (int i = 0; i < p.size(); i++) {
            auto s = p[i];
            std::cout << s << (i < p.size() - 1 ? "," : "");
        }
        std::cout << std::endl;
    }
    for (auto &t : threads) {
        t.join();
    }
    std::cout << "Paths: " << paths.size() << std::endl;
    return 0;
}