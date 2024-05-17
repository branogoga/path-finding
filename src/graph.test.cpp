#include "graph.h"

#include <gtest/gtest.h>

#include <filesystem>

TEST(DefaultGraphLoader, creates_graph_with_4_vertices)
{
  DefaultGraphLoader loader;
  const auto graph = loader.getGraph();
  ASSERT_EQ(graph.m_vertices.size(), 4);
}

TEST(MapGraphLoader, loads_test_data)
{
  MapGraphLoader loader(
      std::filesystem::path(std::string(PROJECT_ROOT_DIR) + "/data/sample_test/test.map").make_preferred().string());
  const auto graph = loader.getGraph();
  ASSERT_EQ(graph.m_vertices.size(), 3);
  // ASSERT_EQ(graph.m_edges.size(), 3);
}

TEST(intersection, returns_all_shared_vertices_of_the_paths)
{
  const Path path1{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  const Path path2{
      7,
      3,
      99,
      1,
      5,
  };
  ASSERT_EQ(intersection(path1, path2), std::vector<Vertex>({1, 3, 5, 7}));
}

TEST(intersection, returns_empty_vector_if_paths_have_no_intersection)
{
  const Path path1{1, 2, 3};
  const Path path2{4, 5, 6};
  ASSERT_EQ(intersection(path1, path2), std::vector<Vertex>({}));
}

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
