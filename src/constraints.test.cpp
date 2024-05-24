#include <gtest/gtest.h>

#include "simulation.h"

TEST(Constraints, initially_no_vertex_is_locked)
{
  DefaultGraphLoader loader;
  WeightedDiGraph graph = loader.getGraph();
  Constraints constraints(graph);
  for (size_t vertex = 0; vertex < graph.m_vertices.size(); ++vertex)
  {
    EXPECT_EQ(constraints.getVertexLock(vertex), std::nullopt);
  }
}

TEST(Constraints, initially_vertices_are_free_for_any_runner)
{
  DefaultGraphLoader loader;
  WeightedDiGraph graph = loader.getGraph();
  Constraints constraints(graph);
  const size_t numberOfRunners = 10;
  for (auto vertex = 0; vertex < graph.m_vertices.size(); ++vertex)
  {
    for (auto runner = 0; runner < numberOfRunners; ++runner)
    {
      EXPECT_TRUE(constraints.isVertexFreeForRunner(vertex, runner));
    }
  }
}

TEST(Constraints, lock_vertex_locks_vertex_to_runner)
{
  DefaultGraphLoader loader;
  WeightedDiGraph graph = loader.getGraph();
  Constraints constraints(graph);
  constraints.lockVertex(1, 7);
  EXPECT_EQ(constraints.getVertexLock(1), 7);
}

TEST(Constraints, lock_vertex_does_not_lock_any_other_vertex)
{
  DefaultGraphLoader loader;
  WeightedDiGraph graph = loader.getGraph();
  Constraints constraints(graph);
  constraints.lockVertex(1, 7);
  EXPECT_EQ(constraints.getVertexLock(0), std::nullopt);
  EXPECT_EQ(constraints.getVertexLock(2), std::nullopt);
  EXPECT_EQ(constraints.getVertexLock(3), std::nullopt);
}

TEST(Constraints, locked_vertex_is_free_for_runner_its_locked_to)
{
  DefaultGraphLoader loader;
  WeightedDiGraph graph = loader.getGraph();
  Constraints constraints(graph);
  constraints.lockVertex(1, 7);
  EXPECT_TRUE(constraints.isVertexFreeForRunner(1, 7));
}

TEST(Constraints, locked_vertex_is_not_free_to_other_runner_then_it_was_locked_to)
{
  DefaultGraphLoader loader;
  WeightedDiGraph graph = loader.getGraph();
  Constraints constraints(graph);
  constraints.lockVertex(1, 7);
  EXPECT_FALSE(constraints.isVertexFreeForRunner(1, 0));
  EXPECT_FALSE(constraints.isVertexFreeForRunner(1, 1));
  EXPECT_FALSE(constraints.isVertexFreeForRunner(1, 2));
  EXPECT_FALSE(constraints.isVertexFreeForRunner(1, 3));
  EXPECT_FALSE(constraints.isVertexFreeForRunner(1, 4));
  EXPECT_FALSE(constraints.isVertexFreeForRunner(1, 5));
  EXPECT_FALSE(constraints.isVertexFreeForRunner(1, 6));
  EXPECT_FALSE(constraints.isVertexFreeForRunner(1, 8));
  EXPECT_FALSE(constraints.isVertexFreeForRunner(1, 9));
  EXPECT_FALSE(constraints.isVertexFreeForRunner(1, 10));
}

TEST(Constraints, unlock_vertex_unlocks_the_vertex)
{
  DefaultGraphLoader loader;
  WeightedDiGraph graph = loader.getGraph();
  Constraints constraints(graph);
  constraints.lockVertex(1, 7);
  constraints.unlockVertex(1);
  EXPECT_EQ(constraints.getVertexLock(1), std::nullopt);
}

TEST(Constraints, unlock_vertex_does_not_unlock_any_other_vertex)
{
  DefaultGraphLoader loader;
  WeightedDiGraph graph = loader.getGraph();
  Constraints constraints(graph);
  for (size_t vertex = 0; vertex < graph.m_vertices.size(); ++vertex)
  {
    constraints.lockVertex(vertex, 7);
  }
  constraints.unlockVertex(1);
  EXPECT_EQ(constraints.getVertexLock(0), 7);
  EXPECT_EQ(constraints.getVertexLock(2), 7);
  EXPECT_EQ(constraints.getVertexLock(3), 7);
}

TEST(Constraints, unlocked_vertex_is_free_for_any_runner)
{
  DefaultGraphLoader loader;
  WeightedDiGraph graph = loader.getGraph();
  Constraints constraints(graph);
  constraints.lockVertex(1, 7);
  constraints.unlockVertex(1);
  const size_t numberOfRunners = 10;
  for (auto runner = 0; runner < numberOfRunners; ++runner)
  {
    EXPECT_TRUE(constraints.isVertexFreeForRunner(1, runner));
  }
}
