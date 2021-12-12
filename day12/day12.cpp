#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <regex>

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

void computePath(std::string end, std::vector<std::string> currentPath, Graph<std::string> graph, std::vector<std::vector<std::string>> &paths) {
    if (currentPath[currentPath.size()-1] == end) {
        paths.push_back(currentPath);
        return;
    }
    for (std::string node : graph.neighbors(currentPath[currentPath.size()-1])) {
        // if it's already in the list, it can't be a small cave (lowercase) because that would cause it to be visited multiple times
        if (!((std::find(currentPath.begin(), currentPath.end(), node) != currentPath.end()) && (std::find_if(node.begin(), node.end(), is_not_alphanum_lower) == node.end()))) {
            auto newPath = currentPath;
            newPath.push_back(node);
            computePath(end, newPath, graph, paths);
        }
    }
}

int main(int argc, char** argv) {
    auto graph = parseInput();
    std::vector<std::vector<std::string>> paths;
    computePath("end", {"start"}, graph, paths);
    for (auto p : paths) {
        for (auto s : p) {
            std::cout << s << ",";
        }
        std::cout << std::endl;
    }
    std::cout << "Paths: " << paths.size() << std::endl;
    return 0;
}