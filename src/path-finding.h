#pragma once

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/astar_search.hpp>
#include <functional>

#include "constraints.h"
#include "graph.h"

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

typedef std::function<Path(
    const WeightedDiGraph& graph,
    const Vertex& start,
    const Vertex& target,
    const Constraints& constraints,
    RunnerId runnerId)>
    MultiAgentShortestPathCalculator;

MultiAgentShortestPathCalculator multi_agent_shortest_path_calculator_wrapper(const ShortestPathCalculator& calculator);

Path space_time_a_star_shortest_path(
    const WeightedDiGraph& graph,
    const Vertex& start,
    const Vertex& target,
    const Constraints& constraints,
    RunnerId runnerId);
