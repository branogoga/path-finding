#ifndef PATHFINDING_GRAPH_H
#define PATHFINDING_GRAPH_H

#include <boost/graph/adjacency_list.hpp>

#include <string>

typedef boost::adjacency_list< boost::listS, boost::vecS, boost::directedS, boost::no_property, boost::property<boost::edge_weight_t, float>> WeightedDiGraph;
typedef boost::graph_traits<WeightedDiGraph>::vertex_descriptor Vertex;
typedef WeightedDiGraph::edge_descriptor Edge;

std::vector<Vertex> shortest_path(const WeightedDiGraph& graph, const Vertex& start, const Vertex& target);
unsigned path_length(const WeightedDiGraph& graph, const std::vector<Vertex>& path);

class GraphLoader {
public:
    virtual WeightedDiGraph getGraph() const = 0;
};

class DefaultGraphLoader : public GraphLoader {
public:
    WeightedDiGraph getGraph() const override;
};

class MapGraphLoader : public GraphLoader {
public:
    MapGraphLoader(const std::string& filename);
    virtual ~MapGraphLoader() = default;

    MapGraphLoader() = delete;
    MapGraphLoader(const MapGraphLoader&) = delete;
    MapGraphLoader& operator=(const MapGraphLoader&) = delete;

    WeightedDiGraph getGraph() const override;

private: 
    WeightedDiGraph readFile();

    std::string filename;
    unsigned width;
    unsigned height;
    std::vector<std::string> map;
    std::vector<std::vector<std::optional<unsigned>>> mapPositionToVertexIndex;
    std::vector<std::pair<size_t, size_t>> vertexIndexToMapPosition;

    WeightedDiGraph graph;
};

#endif //PATHFINDING_GRAPH_H
