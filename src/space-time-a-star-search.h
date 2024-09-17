#include <boost/graph/adjacency_list.hpp>
#include <boost/property_map/property_map.hpp>
#include <iostream>
#include <map>
#include <queue>
#include <vector>

using namespace boost;

// Define graph type
typedef adjacency_list<listS, vecS, directedS, no_property, property<edge_weight_t, int>> Graph;
typedef graph_traits<Graph>::vertex_descriptor Vertex;
typedef graph_traits<Graph>::edge_descriptor Edge;

// Define a state consisting of a vertex and the time
struct State {
  Vertex vertex;
  int time;

  State(Vertex v, int t) : vertex(v), time(t)
  {
  }

  // Compare two states
  bool operator<(const State& other) const
  {
    if (vertex != other.vertex) return vertex < other.vertex;
    return time < other.time;
  }
};

// Define heuristic function
class Heuristic
{
 public:
  Heuristic(Vertex goal) : m_goal(goal)
  {
  }

  int operator()(Vertex v)
  {
    // Example heuristic: Manhattan distance between v and the goal
    return abs(v - m_goal);
  }

 private:
  Vertex m_goal;
};

// Function to check if a vertex is accessible at a given time
bool is_accessible(Vertex v, int time, const std::map<Vertex, std::vector<std::pair<int, int>>>& time_intervals)
{
  if (time_intervals.find(v) == time_intervals.end()) return true;  // No time constraints for this vertex
  for (const auto& interval : time_intervals.at(v))
  {
    if (time >= interval.first && time <= interval.second)
    {
      return true;
    }
  }
  return false;
}

// Custom Space-Time A* implementation with pausing and revisiting
std::vector<Vertex> custom_sta_star(
    const Graph& g,
    Vertex start,
    Vertex goal,
    const Heuristic& heuristic,
    const std::map<Vertex, std::vector<std::pair<int, int>>>& time_intervals)
{
  // Property maps for predecessors, distances, and times
  std::map<State, Vertex> predecessors;
  std::map<State, int> distances;
  std::map<State, int> arrival_times;

  // Priority queue for open list
  typedef std::pair<int, State> PQElement;
  std::priority_queue<PQElement, std::vector<PQElement>, std::greater<PQElement>> open_list;

  // Initialize start vertex
  State start_state(start, 0);
  open_list.push(std::make_pair(0, start_state));
  distances[start_state] = 0;
  arrival_times[start_state] = 0;

  while (!open_list.empty())
  {
    State current_state = open_list.top().second;
    open_list.pop();

    Vertex current_vertex = current_state.vertex;
    int current_time = current_state.time;

    if (current_vertex == goal) break;

    graph_traits<Graph>::out_edge_iterator ei, ei_end;
    for (tie(ei, ei_end) = out_edges(current_vertex, g); ei != ei_end; ++ei)
    {
      Vertex next_vertex = target(*ei, g);
      int weight = get(edge_weight, g, *ei);
      int tentative_distance = distances[current_state] + weight;
      int arrival_time = current_time + weight;

      State next_state(next_vertex, arrival_time);

      if (is_accessible(next_vertex, arrival_time, time_intervals))
      {
        if (distances.find(next_state) == distances.end() || tentative_distance < distances[next_state])
        {
          distances[next_state] = tentative_distance;
          predecessors[next_state] = current_vertex;
          arrival_times[next_state] = arrival_time;
          int priority = tentative_distance + heuristic(next_vertex);
          open_list.push(std::make_pair(priority, next_state));
        }
      }
    }

    // Option to pause at the current vertex (stay for 1 unit of time)
    State paused_state(current_vertex, current_time + 1);
    if (is_accessible(current_vertex, current_time + 1, time_intervals))
    {
      if (distances.find(paused_state) == distances.end() || distances[current_state] < distances[paused_state])
      {
        distances[paused_state] = distances[current_state];
        predecessors[paused_state] = current_vertex;
        arrival_times[paused_state] = current_time + 1;
        open_list.push(std::make_pair(distances[paused_state] + heuristic(current_vertex), paused_state));
      }
    }
  }

  // Reconstruct the shortest path by finding the final goal state
  std::vector<Vertex> path;
  State goal_state(goal, -1);
  int min_time = std::numeric_limits<int>::max();

  for (const auto& entry : arrival_times)
  {
    if (entry.first.vertex == goal && entry.second < min_time)
    {
      goal_state = entry.first;
      min_time = entry.second;
    }
  }

  for (State s = goal_state; s.vertex != start; s = State(predecessors[s], arrival_times[s]))
  {
    path.push_back(s.vertex);
  }
  path.push_back(start);
  std::reverse(path.begin(), path.end());

  return path;
}

int main()
{
  // Define the graph
  Graph g(6);

  // Add edges with weights
  add_edge(0, 1, 5, g);
  add_edge(0, 2, 1, g);
  add_edge(1, 3, 2, g);
  add_edge(2, 1, 2, g);
  add_edge(2, 3, 6, g);
  add_edge(2, 4, 3, g);
  add_edge(3, 4, 7, g);
  add_edge(3, 5, 2, g);
  add_edge(4, 5, 4, g);

  // Define time intervals for vertex accessibility
  std::map<Vertex, std::vector<std::pair<int, int>>> time_intervals;
  time_intervals[3] = {{5, 10}, {15, 20}};  // Vertex 3 is accessible from time 5 to 10 and 15 to 20
  time_intervals[5] = {{0, 15}};            // Vertex 5 is accessible from time 0 to 15

  // Define start and goal vertices
  Vertex start = 0;
  Vertex goal = 5;

  // Run custom Space-Time A* search with pausing and revisiting
  Heuristic heuristic(goal);
  std::vector<Vertex> path = custom_sta_star(g, start, goal, heuristic, time_intervals);

  // Output the shortest path
  std::cout << "Shortest path from " << start << " to " << goal << ":" << std::endl;
  for (Vertex v : path)
  {
    std::cout << v << " ";
  }
  std::cout << std::endl;

  return 0;
}
