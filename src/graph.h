#pragma once

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/astar_search.hpp>
#include <string>

#include "geometry.h"

class VertexNode
{
 public:
  Point2D position;
};

typedef float Distance;
typedef VertexNode VertexProperties;
typedef boost::property<boost::edge_weight_t, Distance> EdgeProperties;
typedef boost::adjacency_list<boost::listS, boost::vecS, boost::directedS, VertexProperties, EdgeProperties>
    WeightedDiGraph;
typedef boost::graph_traits<WeightedDiGraph>::vertex_descriptor Vertex;
typedef WeightedDiGraph::edge_descriptor Edge;

typedef std::vector<Vertex> Path;
std::ostream& operator<<(std::ostream& stream, const Path& path);

class manhattan_distance_heuristic : public boost::astar_heuristic<WeightedDiGraph, Distance>
{
 public:
  manhattan_distance_heuristic(WeightedDiGraph graph, Vertex goal);
  Distance operator()(Vertex v);

 private:
  WeightedDiGraph m_graph;
  Vertex m_goal;
};

// euclidean distance heuristic
class euclidean_distance_heuristic : public boost::astar_heuristic<WeightedDiGraph, Distance>
{
 public:
  euclidean_distance_heuristic(WeightedDiGraph graph, Vertex goal);
  Distance operator()(Vertex v);

 private:
  WeightedDiGraph m_graph;
  Vertex m_goal;
};

typedef std::function<std::vector<Vertex>(const WeightedDiGraph& graph, const Vertex& start)> ShortestPathsCalculator;
std::vector<Vertex> boost_dijkstra_shortest_paths(const WeightedDiGraph& graph, const Vertex& start);
std::vector<Vertex> dijkstra_shortest_paths(const WeightedDiGraph& graph, const Vertex& start);

typedef std::function<Path(const WeightedDiGraph& graph, const Vertex& start, const Vertex& target)>
    ShortestPathCalculator;

Path boost_dijkstra_shortest_path(const WeightedDiGraph& graph, const Vertex& start, const Vertex& target);
Path dijkstra_shortest_path(const WeightedDiGraph& graph, const Vertex& start, const Vertex& target);
Path boost_a_star_shortest_path(const WeightedDiGraph& graph, const Vertex& start, const Vertex& target);
Path a_star_shortest_path(const WeightedDiGraph& graph, const Vertex& start, const Vertex& target);

float path_length(const WeightedDiGraph& graph, const Path& path);
std::vector<Vertex> intersection(const Path& path1, const Path& path2);

class GraphLoader
{
 public:
  virtual WeightedDiGraph getGraph() const = 0;
};

class DefaultGraphLoader : public GraphLoader
{
 public:
  WeightedDiGraph getGraph() const override;
};

class MapGraphLoader : public GraphLoader
{
 public:
  MapGraphLoader(const std::string& filename);
  virtual ~MapGraphLoader() = default;

  MapGraphLoader() = delete;
  MapGraphLoader(const MapGraphLoader&) = delete;
  MapGraphLoader& operator=(const MapGraphLoader&) = delete;

  WeightedDiGraph getGraph() const override;
  const std::string& getFilename() const;
  std::optional<unsigned> convertMapPositionToVertexIndex(size_t row, size_t column) const;
  std::pair<size_t, size_t> convertVertexIndexToMapPosition(unsigned vertex) const;

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
