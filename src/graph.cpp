#include "graph.h"

std::vector<Vertex> extract_path(std::vector<Vertex> predecessor, const Vertex& target, const Vertex& start) {
    std::vector<boost::graph_traits<WeightedDiGraph>::vertex_descriptor > path;
    boost::graph_traits<WeightedDiGraph>::vertex_descriptor current = target;

    while(current!=start)
    {
        path.push_back(current);
        current = predecessor[current];
    }
    path.push_back(start);
    return path;
}

std::vector<Vertex> shortest_path(const WeightedDiGraph& graph, const Vertex& start, const Vertex& target) {
    std::vector<Vertex> predecessor(num_vertices(graph));
    std::vector<int> distance(num_vertices(graph));
    boost::dijkstra_shortest_paths(graph, start, boost::predecessor_map(&predecessor[0]).distance_map(&distance[0]));
    auto path = extract_path(predecessor, target, start);
    return std::vector<Vertex>(path.rbegin(), path.rend());
}

