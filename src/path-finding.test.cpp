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
  Path path = space_time_a_star_shortest_path(
      graph,
      source_vertex,
      target_vertex,
      constraints,
      runnerId /*, euclidean_distance_heuristic(graph, target_vertex)*/);
  EXPECT_EQ(path.size(), 5);
  EXPECT_EQ(path[0], 0);
  EXPECT_EQ(path[1], 2);
  EXPECT_EQ(path[2], 1);
  EXPECT_EQ(path[3], 3);
  EXPECT_EQ(path[4], 5);
}

TEST(shortest_path, space_time_a_star_shortest_path_does_not_plan_a_swap_with_another_runner)
{
  // Line graph 0 <-> 1 <-> 2 <-> 3. Runner 0 has already been planned and locked to travel
  // 0 -> 1 -> 2, i.e. it crosses edge (1,2) during [1,2). Runner 1 starts one step further back, at
  // vertex 3, and wants to travel to vertex 0 - also crossing vertex 1 and edge (1,2)/(2,1). Runner 1
  // has room to wait at vertex 3 until Runner 0 has fully cleared the corridor, so a safe path
  // genuinely exists here. If the planner only checked vertex occupancy (the bug), it could instead
  // schedule Runner 1 to cross edge (2,1) during [1,2) - the exact same time window as Runner 0
  // crossing (1,2) - a head-on collision even though neither runner is ever recorded as occupying
  // the same vertex at the same time.
  WeightedDiGraph graph(4);
  add_edge(0, 1, 1.0f, graph);
  add_edge(1, 0, 1.0f, graph);
  add_edge(1, 2, 1.0f, graph);
  add_edge(2, 1, 1.0f, graph);
  add_edge(2, 3, 1.0f, graph);
  add_edge(3, 2, 1.0f, graph);
  for (size_t index = 0; index < graph.m_vertices.size(); ++index)
  {
    graph.m_vertices[index].m_property.position = {float(index), 0.0f};
  }

  Constraints constraints(graph);
  const RunnerId runner0 = 0;
  const RunnerId runner1 = 1;

  constraints.lockVertex(0, runner0, 0, 1);
  constraints.lockVertex(1, runner0, 1, 2);
  constraints.lockVertex(2, runner0, 2, 3);
  constraints.lockEdge(0, 1, runner0, 0, 1);
  constraints.lockEdge(1, 2, runner0, 1, 2);

  Path path = space_time_a_star_shortest_path(graph, 3, 0, constraints, runner1);

  ASSERT_FALSE(path.empty()) << "A safe path exists (Runner 1 can wait at vertex 3), but none was found.";
  EXPECT_EQ(path.front(), 3u);
  EXPECT_EQ(path.back(), 0u);
  for (unsigned time = 0; time + 1 < path.size(); ++time)
  {
    if (path[time] == path[time + 1])
    {
      continue;  // Runner 1 is waiting in place, not traversing an edge.
    }
    // Whatever move the planner chose, it must not be one that isEdgeFreeForRunner would reject
    // against Runner 0's already-locked schedule, i.e. the plan must be genuinely collision-free.
    EXPECT_TRUE(constraints.isEdgeFreeForRunner(path[time], path[time + 1], runner1, time, time + 1))
        << "Runner 1 swaps with Runner 0 across edge (" << path[time] << "->" << path[time + 1] << ") at time "
        << time;
  }
}

TEST(shortest_path, space_time_a_star_shortest_path_returns_empty_path_if_swapping_is_the_only_option)
{
  // Line graph 0 <-> 1 <-> 2, with no room to retreat: Runner 1 starts exactly where Runner 0 is
  // headed (vertex 2), and vertex 2 has no other neighbor to wait at. Runner 1 must leave vertex 2
  // before Runner 0 arrives there, but the only way out crosses the same edge Runner 0 is using in
  // the opposite direction at that exact moment. There is genuinely no collision-free path here, so
  // the planner must report failure (an empty path) rather than either colliding or hanging.
  WeightedDiGraph graph(3);
  add_edge(0, 1, 1.0f, graph);
  add_edge(1, 0, 1.0f, graph);
  add_edge(1, 2, 1.0f, graph);
  add_edge(2, 1, 1.0f, graph);
  for (size_t index = 0; index < graph.m_vertices.size(); ++index)
  {
    graph.m_vertices[index].m_property.position = {float(index), 0.0f};
  }

  Constraints constraints(graph);
  const RunnerId runner0 = 0;
  const RunnerId runner1 = 1;

  constraints.lockVertex(0, runner0, 0, 1);
  constraints.lockVertex(1, runner0, 1, 2);
  constraints.lockVertex(2, runner0, 2, 3);
  constraints.lockEdge(0, 1, runner0, 0, 1);
  constraints.lockEdge(1, 2, runner0, 1, 2);

  Path path = space_time_a_star_shortest_path(graph, 2, 0, constraints, runner1);

  EXPECT_TRUE(path.empty());
}
