#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <filesystem>
#include <iostream>

#include "graph.h"
#include "scenario.h"
#include "simulation.h"
#include "version.h"

std::string getVersion()
{
  std::ostringstream out;
  out << PathFinding_VERSION_MAJOR << "." << PathFinding_VERSION_MINOR << "." << PathFinding_VERSION_PATCH << "."
      << PathFinding_VERSION_TWEAK;
  return out.str();
}

void print_graph_statistics(/*const*/ WeightedDiGraph &graph)
{
  std::cout << "Graph:" << std::endl;
  std::cout << " - vertices: " << graph.m_vertices.size() << std::endl;
  // std::cout << " - edges: " << graph.m_edges.size() << std::endl;
  // std::cout << " - density: " << static_cast<float>(graph.m_edges.size()) /
  // graph.m_vertices.size() << std::endl;
  std::cout << std::endl;
}

std::string print_graph_to_dot_file(/*const*/ WeightedDiGraph &graph)
{
  std::ostringstream out;
  // represent graph in DOT format and send to cout
  boost::dynamic_properties property_writer;
  property_writer.property("node_id", get(boost::vertex_index, graph));
  property_writer.property("weight", get(boost::edge_weight, graph));
#pragma warning(disable : 4458)  // declaration hides class member
#pragma warning(disable : 4459)  // declaration hides global declaration
  write_graphviz_dp(out, graph, property_writer);
#pragma warning(default : 4459)
#pragma warning(default : 4458)
  return out.str();
}

std::vector<Path> calculate_shortest_paths(
    const std::vector<JobRequest> &jobRequests, const WeightedDiGraph &graph, const unsigned numberOfRobots)
{
  std::vector<Path> paths;
  for (unsigned robotIndex = 0; robotIndex < numberOfRobots; ++robotIndex)
  {
    const auto &jobRequest = jobRequests[robotIndex];
    Vertex start = vertex(jobRequest.startVertex, graph);
    Vertex target = vertex(jobRequest.endVertex, graph);
    paths.emplace_back(boost_dijkstra_shortest_path(graph, start, target));
  }
  return paths;
}

void print_paths(const std::vector<Path> &paths, const WeightedDiGraph &graph)
{
  for (const auto &path : paths)
  {
    std::cout << std::endl;
    const auto start = path[0];
    const auto target = path[path.size() - 1];
    std::cout << "Shortest path from " << start << " to " << target << " (" << path.size() << " vertices, length "
              << path_length(graph, path) << ")" << std::endl;
    // for(const auto& vertex : path) {
    //     std::cout << " - " << vertex << ": position = " <<
    //     graph[vertex].position << std::endl;
    // }
    // std::cout << std::endl;

    // std::cout << "Number of vertices:" << path.size() << std::endl;
    // std::cout << "Total length :" << path_length(graph, path) << std::endl;
  }
}

const std::filesystem::path ProjectRootDirectory = std::filesystem::path(PROJECT_ROOT_DIR).make_preferred();
const std::filesystem::path DataDirectory = ProjectRootDirectory / "data";
const std::filesystem::path SampleTest = DataDirectory / "sample_test" / "test.scen";
const std::filesystem::path Maze_32x32_2_Even_1 = DataDirectory / "maze-32-32-2" / "maze-32-32-2-even-1.scen";

const std::filesystem::path OutputDirectory = ProjectRootDirectory / "output";

int main()
{
  std::cout << "Hello Path Finding " << getVersion() << "!" << std::endl;

  try
  {
    // DefaultScenarioLoader scenarioLoader;
    const std::filesystem::path scenarioFile = Maze_32x32_2_Even_1;
    FileScenarioLoader scenarioLoader(scenarioFile.string());
    const auto jobRequests = scenarioLoader.getjobRequests();
    auto graph = scenarioLoader.getGraph();
    print_graph_statistics(graph);
    {
      const std::string graphDotFileContent = print_graph_to_dot_file(graph);
      std::cout << graphDotFileContent << std::endl;
      std::filesystem::create_directory(OutputDirectory);
      std::ofstream graph_dot_file_stream(OutputDirectory / "graph.dot");
      graph_dot_file_stream << graphDotFileContent << std::endl;
    }

    const unsigned numberOfRobots = 2;  // jobRequests.size();
    const unsigned timeout = (unsigned)1E+06;
    Simulation simulation(jobRequests, graph, numberOfRobots);
    while (!simulation.isFinished() && simulation.getTime() < timeout)
    {
      simulation.advance();
    }

    if (simulation.getTime() >= timeout)
    {
      std::cout << simulation.getTime() << " Simulation timed out." << std::endl;
    }

    if (simulation.isDeadlock())
    {
      std::cout << simulation.getTime() << " Deadlock. Runners wait for each other." << std::endl;
    }

    // const auto start = std::chrono::system_clock::now();
    // std::vector<Path> paths = calculate_shortest_paths(jobRequests, graph,
    // numberOfRobots); const auto end = std::chrono::system_clock::now(); const
    // auto elapsed = end - start; std::cout << "Calculation of " <<
    // numberOfRobots << " paths took " <<
    // std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count() <<
    // "ms" << std::endl; print_paths(paths, graph);

    // typedef std::tuple<unsigned, unsigned, std::vector<Vertex>> Intersection;
    // std::vector<Intersection> intersections;
    // for(unsigned i = 0; i < paths.size(); ++i) {
    //     for(unsigned j=0; j < paths.size(); ++j) {
    //         if(i < j) {
    //             const auto shared_vertices = intersection(paths[i],
    //             paths[j]); if(!shared_vertices.empty()) {
    //                 intersections.emplace_back(i, j, shared_vertices);
    //             }
    //         }
    //     }
    // }

    // std::cout << "Intersections (" << intersections.size() << "):" <<
    // std::endl;
    // // for(const auto& intersection : intersections) {
    // //     const auto [i, j, shared_vertices] = intersection;
    // //     std::cout << " - " << i << ", " << j << ", " << "[";
    // //     for(const auto& vertex : shared_vertices) {
    // //         std::cout << vertex << ", ";
    // //     }
    // //     std::cout << "]" << std::endl;
    // // }
  } catch (std::exception &exception)
  {
    std::cerr << "Uncaught exception: " << exception.what() << std::endl;
  }

  return 0;
}