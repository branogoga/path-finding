#include <gtest/gtest.h>

#include "graph.h"

TEST(MainTest, shortest_path_1)
{
  WeightedDiGraph graph(4);
  add_edge(0, 1, 2.0f, graph);
  add_edge(0, 2, 3.0f, graph);
  add_edge(1, 2, 1.0f, graph);
  add_edge(0, 3, 1.0f, graph);
  add_edge(3, 2, 1.0f, graph);

  Vertex start = vertex(0, graph);
  Vertex target = vertex(2, graph);
  auto path = shortest_path(graph, start, target);

  ASSERT_EQ(3, path.size());
  ASSERT_EQ(0, path[0]);
  ASSERT_EQ(3, path[1]);
  ASSERT_EQ(2, path[2]);
}

TEST(MainTest, shortest_path_2)
{
  WeightedDiGraph graph(4);
  add_edge(0, 1, 1.0f, graph);
  add_edge(0, 2, 3.0f, graph);
  add_edge(1, 2, 1.0f, graph);
  add_edge(0, 3, 2.0f, graph);
  add_edge(3, 2, 1.0f, graph);

  Vertex start = vertex(0, graph);
  Vertex target = vertex(2, graph);
  auto path = shortest_path(graph, start, target);

  ASSERT_EQ(3, path.size());
  ASSERT_EQ(0, path[0]);
  ASSERT_EQ(1, path[1]);
  ASSERT_EQ(2, path[2]);
}

TEST(MainTest, shortest_path_3)
{
  WeightedDiGraph graph(4);
  add_edge(0, 1, 1.0f, graph);
  add_edge(0, 2, 1.0f, graph);
  add_edge(1, 2, 1.0f, graph);
  add_edge(0, 3, 1.0f, graph);
  add_edge(3, 2, 1.0f, graph);

  Vertex start = vertex(0, graph);
  Vertex target = vertex(2, graph);
  auto path = shortest_path(graph, start, target);

  ASSERT_EQ(2, path.size());
  ASSERT_EQ(0, path[0]);
  ASSERT_EQ(2, path[1]);
}

// TODO: Infinite loop.
// TEST(MainTest, shortest_path_not_connected) {
//    WeightedDiGraph graph(4);
//    add_edge(0, 1, 1.0f, graph);
//    add_edge(3, 2, 1.0f, graph);
//
//    Vertex start = vertex(0, graph);
//    Vertex target = vertex(2, graph);
//    auto path = shortest_path(graph, start, target);
//
//    ASSERT_EQ(0, path.size());
//}

// TODO: Negative edge
// TODO: Negative cycle

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}