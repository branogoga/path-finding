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
const std::filesystem::path SituationsColisionCrossCrossThrough =
    std::filesystem::path("situations") / std::filesystem::path("colision-cross") / "cross-through.scen";
const std::filesystem::path SituationsColisionCrossSwapPosition =
    std::filesystem::path("situations") / std::filesystem::path("colision-cross") / "swap-position.scen";
const std::filesystem::path SituationsStepOver =
    std::filesystem::path("situations") / std::filesystem::path("step-over") / "step-over.scen";
const std::filesystem::path SituationsSwapOrder =
    std::filesystem::path("situations") / std::filesystem::path("swap-order") / "swap-order.scen";
const std::filesystem::path Maze_32x32_2_Even_1 = std::filesystem::path("maze-32-32-2") / "maze-32-32-2-even-1.scen";
const std::filesystem::path Maze_128x128_1_Even_1 =
    std::filesystem::path("maze-128-128-1") / "maze-128-128-1-even-1.scen";
const std::filesystem::path Maze_128x128_2_Even_1 =
    std::filesystem::path("maze-128-128-2") / "maze-128-128-2-even-1.scen";
const std::filesystem::path Warehouse_10_20_10_2_1_Even_1 =
    std::filesystem::path("warehouse-10-20-10-2-1") / "warehouse-10-20-10-2-1-even-1.scen";
const std::filesystem::path Warehouse_10_20_10_2_2_Even_1 =
    std::filesystem::path("warehouse-10-20-10-2-2") / "warehouse-10-20-10-2-2-even-1.scen";
const std::filesystem::path Warehouse_20_40_10_2_1_Even_1 =
    std::filesystem::path("warehouse-20-40-10-2-1") / "warehouse-20-40-10-2-1-even-1.scen";
const std::filesystem::path Warehouse_20_40_10_2_2_Even_1 =
    std::filesystem::path("warehouse-20-40-10-2-2") / "warehouse-20-40-10-2-2-even-1.scen";

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

void run_scenario(const std::filesystem::path &scenarioFile)
{
  try
  {
    std::cout << "Processing scenario " << scenarioFile << std::endl;
    FileScenarioLoader scenarioLoader(DataDirectory / scenarioFile);
    const auto jobRequests = scenarioLoader.getjobRequests();
    auto graph = scenarioLoader.getGraph();
    print_graph_statistics(graph);

    const unsigned numberOfRobots =
        std::min<unsigned>({3u, (unsigned)graph.m_vertices.size(), (unsigned)jobRequests.size()});
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
  } catch (std::exception &exception)
  {
    std::cerr << "Uncaught exception: " << exception.what() << std::endl;
  }
}

int main()
{
  std::cout << "Hello Path Finding " << getVersion() << "!" << std::endl;
  std::filesystem::create_directory(OutputDirectory);

  std::vector<std::filesystem::path> scenarioFiles = {
      SampleTest,
      // SituationsColisionCrossCrossThrough,
      // SituationsColisionCrossSwapPosition,
      // SituationsStepOver,
      // SituationsSwapOrder,
      Maze_128x128_1_Even_1,
      Maze_128x128_2_Even_1,
      Maze_32x32_2_Even_1,
      Warehouse_10_20_10_2_1_Even_1,
      Warehouse_10_20_10_2_2_Even_1,
      Warehouse_20_40_10_2_1_Even_1,
      Warehouse_20_40_10_2_2_Even_1};
  for (const auto &scenarioFile : scenarioFiles)
  {
    run_scenario(scenarioFile);
  }

  return 0;
}