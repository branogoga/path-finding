#include "path-finding.h"

#include <algorithm>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/astar_search.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/graph_traits.hpp>
#include <iostream>
#include <queue>

#include "graph.h"

std::vector<Vertex> boost_dijkstra_shortest_paths(const WeightedDiGraph& graph, const Vertex& start)
{
  std::vector<Vertex> predecessor(num_vertices(graph));
  std::vector<float> distance(num_vertices(graph));
  boost::dijkstra_shortest_paths(graph, start, boost::predecessor_map(&predecessor[0]).distance_map(&distance[0]));
  return predecessor;
}

std::vector<Vertex> dijkstra_shortest_paths(const WeightedDiGraph& graph, const Vertex& start)
{
  size_t numberOfVertices = boost::num_vertices(graph);
  std::vector<Vertex> predecessor(numberOfVertices);
  for (Vertex i = 0; i < numberOfVertices; ++i)
  {
    predecessor[i] = i;
  }
  std::vector<Distance> distances(numberOfVertices, std::numeric_limits<Distance>::max());
  distances[start] = 0.0;

  typedef std::pair<Distance, Vertex> Pair;  // (distance, vertex)
  std::priority_queue<Pair, std::vector<Pair>, std::greater<Pair>> priorityQueue;
  priorityQueue.push(std::make_pair(0.0f, start));

  while (!priorityQueue.empty())
  {
    Vertex bestExploredVertex = priorityQueue.top().second;
    Distance distanceToBestExploredVertex = priorityQueue.top().first;
    priorityQueue.pop();

    if (distanceToBestExploredVertex > distances[bestExploredVertex]) continue;

    boost::graph_traits<WeightedDiGraph>::out_edge_iterator edgeIterator, edgeIteratorEnd;
    for (tie(edgeIterator, edgeIteratorEnd) = boost::out_edges(bestExploredVertex, graph);
         edgeIterator != edgeIteratorEnd;
         ++edgeIterator)
    {
      size_t newExploredVertex = target(*edgeIterator, graph);
      Distance weight = boost::get(boost::edge_weight_t(), graph, *edgeIterator);
      if (distances[bestExploredVertex] + weight < distances[newExploredVertex])
      {
        distances[newExploredVertex] = distances[bestExploredVertex] + weight;
        predecessor[newExploredVertex] = bestExploredVertex;
        priorityQueue.push(std::make_pair(distances[newExploredVertex], newExploredVertex));
      }
    }
  }

  return predecessor;
}

Path boost_dijkstra_shortest_path(const WeightedDiGraph& graph, const Vertex& start, const Vertex& target)
{
  std::vector<Vertex> predecessor = boost_dijkstra_shortest_paths(graph, start);
  auto path = extract_path(predecessor, target, start);
  return std::vector<Vertex>(path.rbegin(), path.rend());
}

Path dijkstra_shortest_path(const WeightedDiGraph& graph, const Vertex& start, const Vertex& target)
{
  std::vector<Vertex> predecessor = dijkstra_shortest_paths(graph, start);
  auto path = extract_path(predecessor, target, start);
  return std::vector<Vertex>(path.rbegin(), path.rend());
}

// Define heuristic function
manhattan_distance_heuristic::manhattan_distance_heuristic(WeightedDiGraph graph, Vertex goal)
    : m_graph(graph), m_goal(goal)
{
}

Distance manhattan_distance_heuristic::operator()(Vertex v)
{
  // Example heuristic: Manhattan distance between v and the goal
  Distance dx = std::abs(m_graph[m_goal].position.x - m_graph[v].position.x);
  Distance dy = std::abs(m_graph[m_goal].position.y - m_graph[v].position.y);
  return dx + dy;
}

// euclidean distance heuristic
euclidean_distance_heuristic::euclidean_distance_heuristic(WeightedDiGraph graph, Vertex goal)
    : m_graph(graph), m_goal(goal)
{
}

Distance euclidean_distance_heuristic::operator()(Vertex v)
{
  Distance dx = m_graph[m_goal].position.x - m_graph[v].position.x;
  Distance dy = m_graph[m_goal].position.y - m_graph[v].position.y;
  return ::sqrt(dx * dx + dy * dy);
}

struct found_goal {
};  // exception for termination

// visitor that terminates when we find the goal
class astar_goal_visitor : public boost::default_astar_visitor
{
 public:
  astar_goal_visitor(Vertex goal) : m_goal(goal)
  {
  }
  template <class Graph>
  void examine_vertex(Vertex u, Graph&)
  {
    if (u == m_goal) throw found_goal();
  }

 private:
  Vertex m_goal;
};

Path boost_a_star_shortest_path(const WeightedDiGraph& graph, const Vertex& start, const Vertex& target)
{
  // Create property maps for predecessors and distances
  std::vector<Vertex> predecessors(num_vertices(graph));
  std::vector<int> distances(num_vertices(graph));

  // Run A* search
  // manhattan_distance_heuristic heuristic(graph, target);
  euclidean_distance_heuristic heuristic(graph, target);
  try
  {
    // call astar named parameter interface
    boost::astar_search_tree(
        graph,
        start,
        heuristic,
        predecessor_map(boost::make_iterator_property_map(predecessors.begin(), get(boost::vertex_index, graph)))
            .visitor(astar_goal_visitor(target)));
  } catch (found_goal)
  {  // found a path to the goal
    auto path = extract_path(predecessors, target, start);
    std::reverse(path.begin(), path.end());  // TODO: Why is it reversed resp. why Dijkstra isn't reversed?
    return path;
  }

  std::ostringstream message;
  message << "Unable to find path from " << start << " to " << target << std::endl;
  throw std::runtime_error(message.str());
}

Path a_star_shortest_path(
    const WeightedDiGraph& graph,
    const Vertex& start,
    const Vertex& goal /*, std::function<Distance(Vertex)> heuristic*/)
{
  auto heuristic = euclidean_distance_heuristic(graph, goal);

  // Property maps for predecessors and distances
  std::vector<Vertex> predecessors(num_vertices(graph));
  std::vector<Distance> distances(num_vertices(graph), std::numeric_limits<Distance>::max());

  // Priority queue for open list
  typedef std::pair<Distance, Vertex> Pair;
  std::priority_queue<Pair, std::vector<Pair>, std::greater<Pair>> priorityQueue;

  // Initialize start vertex
  priorityQueue.push(std::make_pair(0.0f, start));
  distances[start] = 0;

  while (!priorityQueue.empty())
  {
    Vertex current_vertex = priorityQueue.top().second;
    priorityQueue.pop();

    if (current_vertex == goal) break;

    boost::graph_traits<WeightedDiGraph>::out_edge_iterator ei, ei_end;
    for (tie(ei, ei_end) = out_edges(current_vertex, graph); ei != ei_end; ++ei)
    {
      Vertex next_vertex = target(*ei, graph);
      Distance weight = boost::get(boost::edge_weight_t(), graph, *ei);
      Distance tentative_distance = distances[current_vertex] + weight;

      if (tentative_distance < distances[next_vertex])
      {
        distances[next_vertex] = tentative_distance;
        predecessors[next_vertex] = current_vertex;
        Distance priority = tentative_distance + heuristic(next_vertex);
        priorityQueue.push(std::make_pair(priority, next_vertex));
      }
    }
  }

  // Reconstruct the shortest path
  std::vector<Vertex> path;
  for (Vertex v = goal; v != start; v = predecessors[v])
  {
    path.push_back(v);
  }
  path.push_back(start);
  std::reverse(path.begin(), path.end());

  return path;
}

struct PositionAtTime {
  Vertex vertex;
  unsigned time;

  PositionAtTime(Vertex vertex, unsigned time) : vertex(vertex), time(time)
  {
  }

  // Compare two states
  bool operator<(const PositionAtTime& other) const
  {
    if (vertex != other.vertex) return vertex < other.vertex;
    return time < other.time;
  }
};

MultiAgentShortestPathCalculator multi_agent_shortest_path_calculator_wrapper(const ShortestPathCalculator& calculator)
{
  return [calculator](
             const WeightedDiGraph& graph,
             const Vertex& start,
             const Vertex& target,
             const Constraints& /*constraints*/,
             RunnerId /*runnerId*/) { return calculator(graph, start, target); };
}

/// See [./doc/coop-path-AIWisdom.pdf](Cooperative Pathinding)
Path space_time_a_star_shortest_path(
    const WeightedDiGraph& graph,
    const Vertex& start,
    const Vertex& goal,
    const Constraints& constraints,
    RunnerId runnerId)
{
  auto heuristic = euclidean_distance_heuristic(graph, goal);

  // Property maps for predecessors and distances
  std::map<PositionAtTime, Vertex> predecessors;
  std::map<PositionAtTime, Distance> distances;
  std::map<PositionAtTime, unsigned> arrival_times;

  // Priority queue for open list
  typedef std::pair<Distance, PositionAtTime> Pair;
  std::priority_queue<Pair, std::vector<Pair>, std::greater<Pair>> priorityQueue;  // openList

  // Initialize start vertex
  PositionAtTime start_state(start, 0u);
  priorityQueue.push(std::make_pair(0.0f, start_state));
  predecessors[start_state] = start;
  distances[start_state] = 0;
  arrival_times[start_state] = 0;

  while (!priorityQueue.empty())
  {
    PositionAtTime current_state = priorityQueue.top().second;
    priorityQueue.pop();
    Vertex current_vertex = current_state.vertex;
    unsigned current_time = current_state.time;

    if (current_vertex == goal) break;

    if (current_time > 10 * graph.m_vertices.size())
    {
      std::cout << "Unable to find path: Time exceeded number of Vertices."
                << "current_time=" << current_time << ", number_of_vertices=" << graph.m_vertices.size() << std::endl;
      continue;
    }

    boost::graph_traits<WeightedDiGraph>::out_edge_iterator ei, ei_end;
    for (tie(ei, ei_end) = out_edges(current_vertex, graph); ei != ei_end; ++ei)
    {
      Vertex next_vertex = target(*ei, graph);
      Distance weight = boost::get(boost::edge_weight_t(), graph, *ei);
      Distance tentative_distance = distances[current_state] + weight;
      unsigned arrival_time = arrival_times[current_state] + 1;

      PositionAtTime next_state(next_vertex, arrival_time);

      if (/*tentative_distance < distances[next_state]
          &&*/
          constraints.isVertexFreeForRunner(next_vertex, runnerId, arrival_time))
      {
        distances[next_state] = tentative_distance;
        predecessors[next_state] = current_vertex;
        arrival_times[next_state] = arrival_time;
        Distance priority = tentative_distance + heuristic(next_vertex);
        priorityQueue.push(std::make_pair(priority, next_state));
      }
    }

    // Allow to pause at the current vertex
    PositionAtTime paused_state(current_vertex, current_time + 1);
    if (constraints.isVertexFreeForRunner(current_vertex, runnerId, current_time + 1))
    {
      distances[paused_state] = distances[current_state];
      predecessors[paused_state] = current_vertex;
      arrival_times[paused_state] = current_time + 1;
      /// Add current_time as penalty for staying at the same place.
      Distance priority = distances[current_state] + heuristic(current_vertex) + current_time;
      priorityQueue.push(std::make_pair(priority, paused_state));
    }
  }

  // Reconstruct the shortest path
  std::vector<Vertex> path;
  PositionAtTime goal_state(goal, 0);
  unsigned min_time = std::numeric_limits<unsigned>::max();
  for (const auto& entry : arrival_times)
  {
    if (entry.first.vertex == goal && entry.second < min_time)
    {
      goal_state = entry.first;
      min_time = entry.second;
    }
  }

  // TODO: return empty path if no path found!

  for (PositionAtTime v = goal_state; v.vertex != start; v = PositionAtTime(predecessors[v], arrival_times[v] - 1))
  {
    path.push_back(v.vertex);
  }
  path.push_back(start);
  std::reverse(path.begin(), path.end());

  return path;
}
