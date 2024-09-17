#include "path-finding.h"

#include <gtest/gtest.h>

#include "sequence.h"

TEST(shortest_paths, boost_dijkstra_shortest_paths)
{
  unsigned num_vertices = 6;  // Number of vertices in the graph
  WeightedDiGraph graph(num_vertices);

  // Adding edges with weights
  add_edge(0, 1, 5, graph);
  add_edge(0, 2, 1, graph);
  add_edge(1, 3, 2, graph);
  add_edge(2, 1, 2, graph);
  add_edge(2, 3, 6, graph);
  add_edge(2, 4, 3, graph);
  add_edge(3, 4, 7, graph);
  add_edge(3, 5, 2, graph);
  add_edge(4, 5, 4, graph);

  unsigned source_vertex = 0;
  std::vector<Vertex> predecessor = boost_dijkstra_shortest_paths(graph, source_vertex);
  EXPECT_EQ(predecessor.size(), num_vertices);
  EXPECT_EQ(predecessor[0], 0);
  EXPECT_EQ(predecessor[1], 2);
  EXPECT_EQ(predecessor[2], 0);
  EXPECT_EQ(predecessor[3], 1);
  EXPECT_EQ(predecessor[4], 2);
  EXPECT_EQ(predecessor[5], 3);
}

TEST(shortest_paths, dijkstra_shortest_paths)
{
  unsigned num_vertices = 6;  // Number of vertices in the graph
  WeightedDiGraph graph(num_vertices);

  // Adding edges with weights
  add_edge(0, 1, 5, graph);
  add_edge(0, 2, 1, graph);
  add_edge(1, 3, 2, graph);
  add_edge(2, 1, 2, graph);
  add_edge(2, 3, 6, graph);
  add_edge(2, 4, 3, graph);
  add_edge(3, 4, 7, graph);
  add_edge(3, 5, 2, graph);
  add_edge(4, 5, 4, graph);

  unsigned source_vertex = 0;
  std::vector<Vertex> predecessor = dijkstra_shortest_paths(graph, source_vertex);
  EXPECT_EQ(predecessor.size(), num_vertices);
  EXPECT_EQ(predecessor[0], 0);
  EXPECT_EQ(predecessor[1], 2);
  EXPECT_EQ(predecessor[2], 0);
  EXPECT_EQ(predecessor[3], 1);
  EXPECT_EQ(predecessor[4], 2);
  EXPECT_EQ(predecessor[5], 3);
}

TEST(shortest_path, boost_dijkstra_shortest_path)
{
  unsigned num_vertices = 6;  // Number of vertices in the graph
  WeightedDiGraph graph(num_vertices);

  // Adding edges with weights
  add_edge(0, 1, 5, graph);
  add_edge(0, 2, 1, graph);
  add_edge(1, 3, 2, graph);
  add_edge(2, 1, 2, graph);
  add_edge(2, 3, 6, graph);
  add_edge(2, 4, 3, graph);
  add_edge(3, 4, 7, graph);
  add_edge(3, 5, 2, graph);
  add_edge(4, 5, 4, graph);

  Vertex source_vertex = 0;
  Vertex target_vertex = 5;
  Path path = boost_dijkstra_shortest_path(graph, source_vertex, target_vertex);
  EXPECT_EQ(path.size(), 5);
  EXPECT_EQ(path[0], 0);
  EXPECT_EQ(path[1], 2);
  EXPECT_EQ(path[2], 1);
  EXPECT_EQ(path[3], 3);
  EXPECT_EQ(path[4], 5);
}

TEST(shortest_path, boost_a_star_shortest_path)
{
  unsigned num_vertices = 6;  // Number of vertices in the graph
  WeightedDiGraph graph(num_vertices);

  // Adding edges with weights
  add_edge(0, 1, 5, graph);
  add_edge(0, 2, 1, graph);
  add_edge(1, 3, 2, graph);
  add_edge(2, 1, 2, graph);
  add_edge(2, 3, 6, graph);
  add_edge(2, 4, 3, graph);
  add_edge(3, 4, 7, graph);
  add_edge(3, 5, 2, graph);
  add_edge(4, 5, 4, graph);

  Vertex source_vertex = 0;
  Vertex target_vertex = 5;
  Path path = boost_a_star_shortest_path(graph, source_vertex, target_vertex);
  EXPECT_EQ(path.size(), 5);
  EXPECT_EQ(path[0], 0);
  EXPECT_EQ(path[1], 2);
  EXPECT_EQ(path[2], 1);
  EXPECT_EQ(path[3], 3);
  EXPECT_EQ(path[4], 5);
}

TEST(manhattan_distance_heuristic, distance)
{
  DefaultGraphLoader loader;
  const auto graph = loader.getGraph();
  manhattan_distance_heuristic heuristic(graph, 3);
  EXPECT_EQ(heuristic(0), 8);  // [0,1] - [3, -4]
}

TEST(euclidean_distance_heuristic, distance)
{
  DefaultGraphLoader loader;
  const auto graph = loader.getGraph();
  euclidean_distance_heuristic heuristic(graph, 3);
  EXPECT_NEAR(heuristic(0), 5.8309f, 1E-03);  // [0,1] - [3, -4]
}

TEST(shortest_path, a_star_shortest_path)
{
  unsigned num_vertices = 6;  // Number of vertices in the graph
  WeightedDiGraph graph(num_vertices);

  // Adding edges with weights
  add_edge(0, 1, 5, graph);
  add_edge(0, 2, 1, graph);
  add_edge(1, 3, 2, graph);
  add_edge(2, 1, 2, graph);
  add_edge(2, 3, 6, graph);
  add_edge(2, 4, 3, graph);
  add_edge(3, 4, 7, graph);
  add_edge(3, 5, 2, graph);
  add_edge(4, 5, 4, graph);

  Vertex source_vertex = 0;
  Vertex target_vertex = 5;
  Path path = a_star_shortest_path(
      graph, source_vertex, target_vertex /*, euclidean_distance_heuristic(graph, target_vertex)*/);
  EXPECT_EQ(path.size(), 5);
  EXPECT_EQ(path[0], 0);
  EXPECT_EQ(path[1], 2);
  EXPECT_EQ(path[2], 1);
  EXPECT_EQ(path[3], 3);
  EXPECT_EQ(path[4], 5);
}

TEST(shortest_path, space_time_a_star_shortest_path)
{
  unsigned num_vertices = 6;  // Number of vertices in the graph
  WeightedDiGraph graph(num_vertices);

  // TODO:
  // - implement GridGraphLoader
  // - try to use on that one
  // - resolve infinite recursion, add stop condition if not able to find path in reasonable amount of steps

  LinearSequence<float> linearSequence;
  AlternateSequence<float> alternateSequence;
  for (size_t index = 0; index < graph.m_vertices.size(); ++index)
  {
    float v = linearSequence();
    float x = v;
    float y = alternateSequence() * (v + 1);
    graph.m_vertices[index].m_property.position = {x, y};
  }

  Constraints constraints(graph);
  RunnerId runnerId = 1;

  // Adding edges with weights
  add_edge(0, 1, 2, graph);
  add_edge(0, 2, 1 /*5*/, graph);
  add_edge(1, 3, 2, graph);
  add_edge(2, 1, 2, graph);
  add_edge(2, 3, 6, graph);
  add_edge(2, 4, 3, graph);
  add_edge(3, 4, 7, graph);
  add_edge(3, 5, 2, graph);
  add_edge(4, 5, 4, graph);

  Vertex source_vertex = 0;
  Vertex target_vertex = 5;
  Path path = space_time_a_star_shortest_path(
      graph,
      source_vertex,
      target_vertex,
      constraints,
      runnerId /*, euclidean_distance_heuristic(graph, target_vertex)*/);
  EXPECT_EQ(path.size(), 4 /*5*/);
  EXPECT_EQ(path[0], 0);
  // EXPECT_EQ(path[1], 2);
  //   EXPECT_EQ(path[2], 1);
  //   EXPECT_EQ(path[3], 3);
  //   EXPECT_EQ(path[4], 5);
  EXPECT_EQ(path[1], 1);
  EXPECT_EQ(path[2], 3);
  EXPECT_EQ(path[3], 5);
}
