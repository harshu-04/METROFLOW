// MetroFlow - Single-File C++ Project
// Optimizes metro routes by stops, time, or cost

#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <queue>
#include <limits>
#include <string>
#include <algorithm>
using namespace std;

struct Edge {
    string to;
    int time;
    double distance;
    int cost;
    string line;

    Edge(const string& t, int ti, double d, int c, const string& l)
        : to(t), time(ti), distance(d), cost(c), line(l) {}
};

unordered_map<string, vector<Edge>> graph;

void loadMetroData(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Failed to open file: " << filename << endl;
        exit(1);
    }

    string line;
    getline(file, line); // skip header

    while (getline(file, line)) {
        stringstream ss(line);
        string from, to, timeStr, distStr, costStr, lineName;

        getline(ss, from, ',');
        getline(ss, to, ',');
        getline(ss, timeStr, ',');
        getline(ss, distStr, ',');
        getline(ss, costStr, ',');
        getline(ss, lineName, ',');

        try {
            int time = stoi(timeStr);
            double distance = stod(distStr);
            int cost = stoi(costStr);

            graph[from].push_back(Edge(to, time, distance, cost, lineName));
            graph[to].push_back(Edge(from, time, distance, cost, lineName));
        } catch (const invalid_argument& e) {
            cerr << "Skipping invalid line: " << line << endl;
            continue;
        }
    }
    file.close();
}

string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, last - first + 1);
}

vector<string> bfsLeastStops(const string& start, const string& end) {
    unordered_map<string, string> parent;
    unordered_map<string, bool> visited;
    queue<string> q;
    visited[start] = true;
    q.push(start);

    while (!q.empty()) {
        string curr = q.front(); q.pop();
        if (curr == end) break;

        for (const auto& edge : graph[curr]) {
            if (!visited[edge.to]) {
                visited[edge.to] = true;
                parent[edge.to] = curr;
                q.push(edge.to);
            }
        }
    }

    vector<string> path;
    string current = end;
    while (current != start && parent.find(current) != parent.end()) {
        path.push_back(current);
        current = parent[current];
    }
    if (current == start) {
        path.push_back(start);
        reverse(path.begin(), path.end());
    } else {
        path.clear(); // no path found
    }
    return path;
}

vector<string> dijkstra(const string& start, const string& end, const string& criteria) {
    unordered_map<string, int> cost;
    unordered_map<string, string> parent;
    auto cmp = [](pair<int, string>& a, pair<int, string>& b) { return a.first > b.first; };
    priority_queue<pair<int, string>, vector<pair<int, string>>, decltype(cmp)> pq(cmp);

    for (auto& entry : graph) {
        cost[entry.first] = numeric_limits<int>::max();
    }
    cost[start] = 0;
    pq.push({0, start});

    while (!pq.empty()) {
        auto top = pq.top(); pq.pop();
        int currCost = top.first;
        string curr = top.second;

        if (curr == end) break;

        for (const auto& edge : graph[curr]) {
            int weight = (criteria == "time") ? edge.time : edge.cost;
            if (cost[curr] + weight < cost[edge.to]) {
                cost[edge.to] = cost[curr] + weight;
                parent[edge.to] = curr;
                pq.push({cost[edge.to], edge.to});
            }
        }
    }

    vector<string> path;
    string current = end;
    while (current != start && parent.find(current) != parent.end()) {
        path.push_back(current);
        current = parent[current];
    }
    if (current == start) {
        path.push_back(start);
        reverse(path.begin(), path.end());
    } else {
        path.clear(); // no path found
    }
    return path;
}

int main() {
    loadMetroData("modidata.csv");
   

    cout << "\nWelcome to MetroFlow - Metro Route Optimization System" << endl;
    cout << "Enter Source Station: ";
    string source; getline(cin, source);
    source = trim(source);

    cout << "Enter Destination Station: ";
    string destination; getline(cin, destination);
    destination = trim(destination);

    cout << "Choose optimization criteria:\n1. Least Stops\n2. Least Cost\n3. Least Time\nYour choice: ";
    int choice; cin >> choice;
    cin.ignore();

    vector<string> path;
    string criteria;

    if (choice == 1) {
        path = bfsLeastStops(source, destination);
        criteria = "Least Stops";
    } else if (choice == 2) {
        path = dijkstra(source, destination, "cost");
        criteria = "Least Cost";
    } else if (choice == 3) {
        path = dijkstra(source, destination, "time");
        criteria = "Least Time";
    } else {
        cout << "Invalid choice.\n";
        return 1;
    }

    if (path.empty()) {
        cout << "\nNo path found between the given stations.\n";
    } else {
        cout << "\nOptimal Path based on " << criteria << ":\n";
        for (const string& station : path) {
            cout << station;
            if (station != path.back()) cout << " -> ";
        }
        cout << endl;
    }

    return 0;
}
