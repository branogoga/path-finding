#ifndef PATHFINDING_GRAPH_H
#define PATHFINDING_GRAPH_H

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

typedef boost::adjacency_list< boost::listS, boost::vecS, boost::directedS, boost::no_property, boost::property<boost::edge_weight_t, float>> WeightedDiGraph;
typedef boost::graph_traits<WeightedDiGraph>::vertex_descriptor Vertex;

std::vector<Vertex> extract_path(std::vector<Vertex> predecessor, const Vertex& target, const Vertex& start);
std::vector<Vertex> shortest_path(const WeightedDiGraph& graph, const Vertex& start, const Vertex& target);

#endif //PATHFINDING_GRAPH_H
