#include "graphviz.h"

#include <boost/graph/graphviz.hpp>
#include <filesystem>

#include "color.h"

std::vector<std::string> getVertexColors(const WeightedDiGraph &graph, const std::vector<Runner> &runners)
{
  const size_t numberOfVertices = num_vertices(graph);

  std::vector<std::string> vertexColors(numberOfVertices, "");
  for (auto runner : runners)
  {
    const auto color = getGraphVizColor(runner.getId());
    for (auto vertex : runner.getRemainingPath())
    {
      vertexColors[vertex] = color;
    }
  }

  for (auto &color : vertexColors)
  {
    if (color.size() == 0)
    {
      color = "none";
    }
  }

  return vertexColors;
}

typedef std::pair<size_t, size_t> EdgeKeyType;
std::map<EdgeKeyType, std::string> getEdgeColors(const std::vector<Runner> &runners)
{
  std::map<EdgeKeyType, std::string> edge_colors;
  for (auto runner : runners)
  {
    const auto color = getGraphVizColor(runner.getId());
    const auto remainingPath = runner.getRemainingPath();
    for (auto index = 0; index < remainingPath.size() - 1; ++index)
    {
      edge_colors[{remainingPath[index], remainingPath[index + 1]}] = color;
    }
  }
  return edge_colors;
}

std::string print_graph_to_dot_file(WeightedDiGraph &graph, const std::vector<Runner> &runners = {})
{
  std::vector<std::string> vertexColors = getVertexColors(graph, runners);
  std::map<EdgeKeyType, std::string> edge_colors = getEdgeColors(runners);

  std::ostringstream out;

  auto vertex_writer = [&graph, &vertexColors](std::ostream &out, const WeightedDiGraph::vertex_descriptor &v)
  {
    out << "[pin=true, pos=\"" << graph[v].position.x << "," << graph[v].position.y << "!\", style=wedged, fillcolor=\""
        << vertexColors[v] << ":" << vertexColors[v] << "\"]";
  };

  auto edge_writer = [&graph, &edge_colors](std::ostream &out, const WeightedDiGraph::edge_descriptor &e)
  {
    auto colorIterator = edge_colors.find({e.m_source, e.m_target});
    auto color = colorIterator != edge_colors.end() ? colorIterator->second : "black";
    out << "[weight=\"" << boost::get(boost::edge_weight_t(), graph, e) << "\", color=\"" << color << "\"]";
  };

#pragma warning(disable : 4458)  // declaration hides class member
#pragma warning(disable : 4459)  // declaration hides global declaration
  write_graphviz(out, graph, vertex_writer, edge_writer);
#pragma warning(default : 4459)
#pragma warning(default : 4458)
  return out.str();
}

void write_graph_to_dot_file(
    const std::filesystem::path &filename, WeightedDiGraph &graph, const std::vector<Runner> &runners)
{
  const std::string graphDotFileContent = print_graph_to_dot_file(graph, runners);
  std::ofstream graph_dot_file_stream(filename);
  graph_dot_file_stream << graphDotFileContent << std::endl;
}
