#include "graph.h"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/astar_search.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/graph_traits.hpp>
#include <filesystem>
#include <fstream>
#include <optional>
#include <queue>

#include "sequence.h"

bool isNear(const Point2D& p1, const Point2D& p2, const float precision)
{
  return std::abs(p2.x - p1.x) < precision && std::abs(p2.y - p1.y) < precision;
}

bool operator==(const Point2D& p1, const Point2D& p2)
{
  return p1.x == p2.x && p1.y == p2.y;
}

std::ostream& operator<<(std::ostream& stream, const Point2D& point)
{
  stream << "[ " << point.x << "," << point.y << " ]";
  return stream;
}

std::ostream& operator<<(std::ostream& stream, const Path& path)
{
  stream << "[";
  for (auto vertex : path)
  {
    stream << vertex << ", " << std::endl;
  }
  stream << "]";
  return stream;
}

Path extract_path(std::vector<Vertex> predecessor, const Vertex& target, const Vertex& start)
{
  std::vector<boost::graph_traits<WeightedDiGraph>::vertex_descriptor> path;
  boost::graph_traits<WeightedDiGraph>::vertex_descriptor current = target;

  while (current != start)
  {
    path.push_back(current);
    current = predecessor[current];
  }
  path.push_back(start);
  return path;
}

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

// Define heuristic function
class manhattan_distance_heuristic : public boost::astar_heuristic<WeightedDiGraph, Distance>
{
 public:
  manhattan_distance_heuristic(WeightedDiGraph graph, Vertex goal) : m_graph(graph), m_goal(goal)
  {
  }

  Distance operator()(Vertex v)
  {
    // Example heuristic: Manhattan distance between v and the goal
    Distance dx = std::abs(m_graph[m_goal].position.x - m_graph[v].position.x);
    Distance dy = std::abs(m_graph[m_goal].position.y - m_graph[v].position.y);
    return dx + dy;
  }

 private:
  WeightedDiGraph m_graph;
  Vertex m_goal;
};

// euclidean distance heuristic
class euclidean_distance_heuristic : public boost::astar_heuristic<WeightedDiGraph, Distance>
{
 public:
  euclidean_distance_heuristic(WeightedDiGraph graph, Vertex goal) : m_graph(graph), m_goal(goal)
  {
  }
  Distance operator()(Vertex v)
  {
    Distance dx = m_graph[m_goal].position.x - m_graph[v].position.x;
    Distance dy = m_graph[m_goal].position.y - m_graph[v].position.y;
    return ::sqrt(dx * dx + dy * dy);
  }

 private:
  WeightedDiGraph m_graph;
  Vertex m_goal;
};

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

float path_length(const WeightedDiGraph& graph, const Path& path)
{
  float length = 0;
  for (size_t index = 0; index < path.size() - 1; ++index)
  {
    const std::pair<Edge, bool> edgeDescriptor = boost::edge(path[index], path[index + 1], graph);
    if (edgeDescriptor.second)
    {
      length += boost::get(boost::edge_weight_t(), graph, edgeDescriptor.first);
    }
  }
  return length;
}

/// @brief Returns shared vertices of two paths. Time complexity O(N*logN), where N is max. path length.
/// @param path1
/// @param path2
/// @return Sorted list of shared vertices
std::vector<Vertex> intersection(const Path& path1, const Path& path2)
{
  Path vertices1 = path1;
  Path vertices2 = path2;
  std::sort(vertices1.begin(), vertices1.end());
  std::sort(vertices2.begin(), vertices2.end());

  std::vector<Vertex> intersection;
  std::set_intersection(
      vertices1.begin(), vertices1.end(), vertices2.begin(), vertices2.end(), std::back_inserter(intersection));

  return intersection;
}

WeightedDiGraph DefaultGraphLoader::getGraph() const
{
  WeightedDiGraph graph(4);

  LinearSequence<float> linearSequence;
  AlternateSequence<float> alternateSequence;

  for (size_t index = 0; index < graph.m_vertices.size(); ++index)
  {
    float v = linearSequence();
    float x = v;
    float y = alternateSequence() * (v + 1);
    graph.m_vertices[index].m_property.position = {x, y};
  }

  add_edge(0, 1, 2.0f, graph);
  add_edge(0, 2, 3.0f, graph);
  add_edge(1, 2, 1.0f, graph);
  add_edge(0, 3, 1.0f, graph);
  add_edge(3, 2, 1.0f, graph);

  return graph;
}

bool isVertexPassable(char value)
{
  return value == '.' || value == 'G';
}

void readStaticHeaderLine(std::ifstream& file, const std::string expectedLine, const std::string& filename)
{
  std::string line;
  std::getline(file, line);
  if (line != expectedLine)
  {
    std::ostringstream message;
    message << "Failed to open '" << filename << "': invalid format. Expected 'type octile', got '" << line << "'";
    throw std::invalid_argument(message.str());
  }
}

unsigned readUnsignedValue(std::ifstream& file, const std::string expectedParameterTitle, const std::string& filename)
{
  std::string line;
  std::getline(file, line);
  std::string label;
  std::string stringValue;
  std::istringstream lineStream(line);
  std::getline(lineStream, label, ' ');
  if (label != expectedParameterTitle)
  {
    std::ostringstream message;
    message << "Failed to open '" << filename << "': invalid format. Expected 'type octile', got '" << label << "'";
    throw std::invalid_argument(message.str());
  }
  std::getline(lineStream, stringValue, ' ');
  int value = std::stoi(stringValue);
  if (value < 0)
  {
    std::ostringstream message;
    message << "Failed to open '" << filename << "': invalid format. Expected height to be positive, got '" << value
            << "'";
    throw std::invalid_argument(message.str());
  }
  return static_cast<unsigned>(value);
}

std::vector<std::string> readMap(std::ifstream& file, unsigned width, unsigned height, const std::string& filename)
{
  std::vector<std::string> map;
  map.reserve(height);
  for (unsigned h = 0; h < height; ++h)
  {
    std::string line;
    std::getline(file, line);
    map.push_back(line);
    if (line.size() != width)
    {
      std::ostringstream message;
      message << "Failed to open '" << filename << "': invalid format. Expected map row " << h << " will have " << width
              << " characters, but got " << line.size();
      throw std::invalid_argument(message.str());
    }
  }
  return map;
}

MapGraphLoader::MapGraphLoader(const std::string& filename) : filename(filename), width(0), height(0)
{
  graph = readFile();
}

WeightedDiGraph MapGraphLoader::getGraph() const
{
  return graph;
}

const std::string& MapGraphLoader::getFilename() const
{
  return filename;
}

std::optional<unsigned> MapGraphLoader::convertMapPositionToVertexIndex(size_t row, size_t column) const
{
  return mapPositionToVertexIndex[row][column];
}

std::pair<size_t, size_t> MapGraphLoader::convertVertexIndexToMapPosition(unsigned vertex) const
{
  return vertexIndexToMapPosition[vertex];
}

// Map Format (https://movingai.com/benchmarks/formats.html)
//
// The maps have the following format :
// All maps begin with the lines :
//
// type octile
// height y
// width x
// map
// where y and x are the respective height and width of the map.
// The map data is store as an ASCII grid.The upper - left corner of the map is(0, 0).The following characters are
// possible :
//
// . - passable terrain
// G - passable terrain
// @ - out of bounds
// O - out of bounds
// T - trees(unpassable)
// S - swamp(passable from regular terrain)
// W - water(traversable, but not passable from terrain)
WeightedDiGraph MapGraphLoader::readFile()
{
  std::ifstream file(filename);
  if (!file.is_open())
  {
    std::ostringstream message;
    message << "Failed to open '" << filename << "' from " << std::filesystem::current_path();
    throw std::runtime_error(message.str());
  }

  readStaticHeaderLine(file, "type octile", filename);
  height = readUnsignedValue(file, "height", filename);
  width = readUnsignedValue(file, "width", filename);
  readStaticHeaderLine(file, "map", filename);

  map = readMap(file, width, height, filename);

  mapPositionToVertexIndex = std::vector<std::vector<std::optional<unsigned>>>(
      height, std::vector<std::optional<unsigned>>(width, std::nullopt));
  vertexIndexToMapPosition = std::vector<std::pair<size_t, size_t>>();
  unsigned numberOfPassableNodes = 0;
  for (size_t row = 0; row < height; ++row)
  {
    for (size_t column = 0; column < width; ++column)
    {
      const char& value = map[row][column];
      if (isVertexPassable(value))
      {
        vertexIndexToMapPosition.emplace_back(std::make_pair(row, column));
        mapPositionToVertexIndex[row][column] = numberOfPassableNodes;
        ++numberOfPassableNodes;
      }
    }
  }

  WeightedDiGraph loadedGraph(numberOfPassableNodes);
  for (size_t row = 0; row < height; ++row)
  {
    for (size_t column = 0; column < width; ++column)
    {
      const char& value = map[row][column];
      if (isVertexPassable(value))
      {
        size_t currentVertexIndex = *mapPositionToVertexIndex[row][column];
        loadedGraph.m_vertices[currentVertexIndex].m_property.position = {float(row), float(column)};
        // Add edge to the left
        if (column > 0 && isVertexPassable(map[row][column - 1]))
        {
          std::optional<unsigned> leftVertexIndex = mapPositionToVertexIndex[row][column - 1];
          if (leftVertexIndex)
          {
            add_edge(currentVertexIndex, *leftVertexIndex, 1.0f, loadedGraph);
            add_edge(*leftVertexIndex, currentVertexIndex, 1.0f, loadedGraph);
          }
        }
        // Add edge to the top
        if (row > 0 && isVertexPassable(map[row - 1][column]))
        {
          std::optional<unsigned> topVertexIndex = mapPositionToVertexIndex[row - 1][column];
          if (topVertexIndex)
          {
            add_edge(currentVertexIndex, *topVertexIndex, 1.0f, loadedGraph);
            add_edge(*topVertexIndex, currentVertexIndex, 1.0f, loadedGraph);
          }
        }
      }
    }
  }
  return loadedGraph;
}
