#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <filesystem>
#include <iostream>
#include <map>

#include "color.h"
#include "graph.h"
#include "graphviz.h"
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

const std::filesystem::path ProjectRootDirectory = std::filesystem::path(PROJECT_ROOT_DIR).make_preferred();

const std::filesystem::path DataDirectory = ProjectRootDirectory / "data";
const std::filesystem::path OutputDirectory = ProjectRootDirectory / "output";

const std::filesystem::path SampleTest = std::filesystem::path("sample_test") / "test.scen";
const std::filesystem::path Maze_32x32_2_Even_1 = std::filesystem::path("maze-32-32-2") / "maze-32-32-2-even-1.scen";

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

int main()
{
  std::cout << "Hello Path Finding " << getVersion() << "!" << std::endl;
  std::filesystem::create_directory(OutputDirectory);

  try
  {
    // DefaultScenarioLoader scenarioLoader;
    const std::filesystem::path scenarioFile = Maze_32x32_2_Even_1;
    FileScenarioLoader scenarioLoader(DataDirectory / scenarioFile);
    const auto jobRequests = scenarioLoader.getjobRequests();
    auto graph = scenarioLoader.getGraph();
    print_graph_statistics(graph);

    const unsigned numberOfRobots = 3;  // jobRequests.size();
    const unsigned timeout = (unsigned)1E+06;
    Simulation simulation(jobRequests, graph, numberOfRobots);
    simulation.advance();
    const auto scenarioDirectory = std::filesystem::path(scenarioFile).remove_filename();
    std::filesystem::create_directory(OutputDirectory / scenarioDirectory);
    write_graph_to_dot_file(OutputDirectory / scenarioDirectory / "graph.dot", graph, simulation.getRunners());

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
      write_graph_to_dot_file(OutputDirectory / scenarioDirectory / "deadlock.dot", graph, simulation.getRunners());
      std::cout << simulation.getTime() << " Deadlock. Runners wait for each other." << std::endl;
    }

    std::cout << simulation.getNewJobRequests().size() << " jobs was not started." << std::endl;
    std::cout << simulation.getJobAssignments().size() << " jobs is assigned." << std::endl;
    std::cout << simulation.getFinishedJobRequests().size() << " jobs was finished." << std::endl;

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