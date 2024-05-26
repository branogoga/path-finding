#include "constraints.h"

#include <gtest/gtest.h>

#include "simulation.h"

class ConstraintsStub : public Constraints
{
 public:
  ConstraintsStub(const WeightedDiGraph& graph) : Constraints(graph)
  {
  }

  typedef Constraints::VertexLockIntervalType VertexLockIntervalType;
  typedef Constraints::VertexLocksType VertexLocksType;
  const VertexLocksType& getVertexLocks(Vertex vertex) const
  {
    return locks[vertex];
  }
};

const Vertex defaultVertex = 1;
const Vertex otherVertex = 2;
const RunnerId defaultRunner = 7;
const RunnerId otherRunner = 3;

TEST(Constraints, initially_no_vertex_is_locked)
{
  DefaultGraphLoader loader;
  WeightedDiGraph graph = loader.getGraph();
  ConstraintsStub constraints(graph);
  for (size_t vertex = 0; vertex < graph.m_vertices.size(); ++vertex)
  {
    EXPECT_TRUE(constraints.getVertexLocks(vertex).empty());
  }
}

TEST(Constraints, initially_vertices_are_free_for_any_runner_during_infinite_time)
{
  DefaultGraphLoader loader;
  WeightedDiGraph graph = loader.getGraph();
  ConstraintsStub constraints(graph);
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
  ConstraintsStub constraints(graph);
  EXPECT_TRUE(constraints.lockVertex(defaultVertex, defaultRunner));
  EXPECT_EQ(constraints.getVertexLock(defaultVertex, 0), defaultRunner);
}

TEST(Constraints, lock_vertex_locks_vertex_to_runner_for_infinite_time_if_time_interval_not_specified)
{
  DefaultGraphLoader loader;
  WeightedDiGraph graph = loader.getGraph();
  ConstraintsStub constraints(graph);
  EXPECT_TRUE(constraints.lockVertex(defaultVertex, defaultRunner));
  const auto& vertexLocks = constraints.getVertexLocks(1);
  auto numberOfIntervals = vertexLocks.iterative_size();
  EXPECT_EQ(numberOfIntervals, 1u);
  for (auto it = vertexLocks.begin(); it != vertexLocks.end(); ++it)
  {
    EXPECT_EQ(it->first.lower(), std::numeric_limits<unsigned>::min());
    EXPECT_EQ(it->first.upper(), std::numeric_limits<unsigned>::max());
    ConstraintsStub::VertexLockIntervalType lockedToSet = it->second;
    EXPECT_EQ(lockedToSet.size(), 1);
    EXPECT_EQ(*lockedToSet.begin(), defaultRunner);
  }
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, defaultRunner));
  EXPECT_FALSE(constraints.isVertexFreeForRunner(defaultVertex, otherRunner));
}

TEST(Constraints, lock_vertex_locks_vertex_to_runner_since_start_time_if_only_start_time_specified)
{
  const unsigned lockedSince = 3;
  DefaultGraphLoader loader;
  WeightedDiGraph graph = loader.getGraph();
  ConstraintsStub constraints(graph);
  EXPECT_TRUE(constraints.lockVertex(defaultVertex, defaultRunner, lockedSince));
  const auto& vertexLocks = constraints.getVertexLocks(1);
  auto numberOfIntervals = vertexLocks.iterative_size();
  EXPECT_EQ(numberOfIntervals, 1u);
  for (auto it = vertexLocks.begin(); it != vertexLocks.end(); ++it)
  {
    EXPECT_EQ(it->first.lower(), lockedSince);
    EXPECT_EQ(it->first.upper(), std::numeric_limits<unsigned>::max());
    ConstraintsStub::VertexLockIntervalType lockedToSet = it->second;
    EXPECT_EQ(lockedToSet.size(), 1);
    EXPECT_EQ(*lockedToSet.begin(), defaultRunner);
  }
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, otherRunner, 0, lockedSince));
  EXPECT_FALSE(constraints.isVertexFreeForRunner(defaultVertex, otherRunner, lockedSince, 4));
}

TEST(Constraints, lock_vertex_locks_vertex_to_runner_for_specified_time_interval)
{
  const unsigned lockedSince = 3;
  const unsigned lockedTill = 11;
  DefaultGraphLoader loader;
  WeightedDiGraph graph = loader.getGraph();
  ConstraintsStub constraints(graph);
  EXPECT_TRUE(constraints.lockVertex(defaultVertex, defaultRunner, lockedSince, lockedTill));
  const auto& vertexLocks = constraints.getVertexLocks(1);
  auto numberOfIntervals = vertexLocks.iterative_size();
  EXPECT_EQ(numberOfIntervals, 1u);
  for (auto it = vertexLocks.begin(); it != vertexLocks.end(); ++it)
  {
    EXPECT_EQ(it->first.lower(), lockedSince);
    EXPECT_EQ(it->first.upper(), lockedTill);
    ConstraintsStub::VertexLockIntervalType lockedToSet = it->second;
    EXPECT_EQ(lockedToSet.size(), 1);
    EXPECT_EQ(*lockedToSet.begin(), defaultRunner);
  }
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, otherRunner, 0, lockedSince));
  EXPECT_FALSE(constraints.isVertexFreeForRunner(defaultVertex, otherRunner, lockedSince, lockedTill));
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, otherRunner, lockedTill));
}

TEST(Constraints, lock_vertex_with_same_interval_and_runner)
{
  const unsigned lockedSince = 3;
  const unsigned lockedTill = 11;
  DefaultGraphLoader loader;
  WeightedDiGraph graph = loader.getGraph();
  ConstraintsStub constraints(graph);
  EXPECT_TRUE(constraints.lockVertex(defaultVertex, defaultRunner, lockedSince, lockedTill));
  EXPECT_TRUE(constraints.lockVertex(defaultVertex, defaultRunner, lockedSince, lockedTill));
  const auto& vertexLocks = constraints.getVertexLocks(1);
  auto numberOfIntervals = vertexLocks.iterative_size();
  EXPECT_EQ(numberOfIntervals, 1u);
  for (auto it = vertexLocks.begin(); it != vertexLocks.end(); ++it)
  {
    EXPECT_EQ(it->first.lower(), lockedSince);
    EXPECT_EQ(it->first.upper(), lockedTill);
    ConstraintsStub::VertexLockIntervalType lockedToSet = it->second;
    EXPECT_EQ(lockedToSet.size(), 1);
    EXPECT_EQ(*lockedToSet.begin(), defaultRunner);
  }
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, otherRunner, 0, lockedSince));
  EXPECT_FALSE(constraints.isVertexFreeForRunner(defaultVertex, otherRunner, lockedSince, lockedTill));
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, otherRunner, lockedTill));
}

TEST(Constraints, lock_vertex_with_overlaping_interval_and_same_runner)
{
  const unsigned lockedSince1 = 3;
  const unsigned lockedTill1 = 11;
  const unsigned lockedSince2 = 7;
  const unsigned lockedTill2 = 23;
  DefaultGraphLoader loader;
  WeightedDiGraph graph = loader.getGraph();
  ConstraintsStub constraints(graph);
  EXPECT_TRUE(constraints.lockVertex(defaultVertex, defaultRunner, lockedSince1, lockedTill1));
  EXPECT_TRUE(constraints.lockVertex(defaultVertex, defaultRunner, lockedSince2, lockedTill2));
  const auto& vertexLocks = constraints.getVertexLocks(1);
  auto numberOfIntervals = vertexLocks.iterative_size();
  EXPECT_EQ(numberOfIntervals, 1u);
  for (auto it = vertexLocks.begin(); it != vertexLocks.end(); ++it)
  {
    EXPECT_EQ(it->first.lower(), lockedSince1);
    EXPECT_EQ(it->first.upper(), lockedTill2);
    ConstraintsStub::VertexLockIntervalType lockedToSet = it->second;
    EXPECT_EQ(lockedToSet.size(), 1);
    EXPECT_EQ(*lockedToSet.begin(), defaultRunner);
  }
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, otherRunner, 0, lockedSince1));
  EXPECT_FALSE(constraints.isVertexFreeForRunner(defaultVertex, otherRunner, lockedSince1, lockedTill2));
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, otherRunner, lockedTill2));
}

TEST(Constraints, lock_vertex_with_overlaping_interval_and_different_runner)
{
  const unsigned lockedSince1 = 3;
  const unsigned lockedTill1 = 11;
  const unsigned lockedSince2 = 7;
  const unsigned lockedTill2 = 23;
  DefaultGraphLoader loader;
  WeightedDiGraph graph = loader.getGraph();
  ConstraintsStub constraints(graph);
  EXPECT_TRUE(constraints.lockVertex(defaultVertex, defaultRunner, lockedSince1, lockedTill1));
  EXPECT_FALSE(constraints.lockVertex(defaultVertex, otherRunner, lockedSince2, lockedTill2));
  const auto& vertexLocks = constraints.getVertexLocks(1);
  auto numberOfIntervals = vertexLocks.iterative_size();
  EXPECT_EQ(numberOfIntervals, 1u);
  for (auto it = vertexLocks.begin(); it != vertexLocks.end(); ++it)
  {
    EXPECT_EQ(it->first.lower(), lockedSince1);
    EXPECT_EQ(it->first.upper(), lockedTill1);
    ConstraintsStub::VertexLockIntervalType lockedToSet = it->second;
    EXPECT_EQ(lockedToSet.size(), 1);
    EXPECT_EQ(*lockedToSet.begin(), defaultRunner);
  }
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, otherRunner, 0, lockedSince1));
  EXPECT_FALSE(constraints.isVertexFreeForRunner(defaultVertex, otherRunner, lockedSince1, lockedTill1));
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, otherRunner, lockedTill1));
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, defaultRunner, 0, lockedSince1));
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, defaultRunner, lockedSince1, lockedTill1));
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, defaultRunner, lockedTill1));
}

TEST(Constraints, lock_vertex_with_same_interval_and_different_runner_returns_false_and_preserves_previous_lock)
{
  const unsigned lockedSince = 3;
  const unsigned lockedTill = 11;
  DefaultGraphLoader loader;
  WeightedDiGraph graph = loader.getGraph();
  ConstraintsStub constraints(graph);
  EXPECT_TRUE(constraints.lockVertex(defaultVertex, defaultRunner, lockedSince, lockedTill));
  EXPECT_FALSE(constraints.lockVertex(defaultVertex, otherRunner, lockedSince, lockedTill));
  const auto& vertexLocks = constraints.getVertexLocks(1);
  auto numberOfIntervals = vertexLocks.iterative_size();
  EXPECT_EQ(numberOfIntervals, 1u);
  for (auto it = vertexLocks.begin(); it != vertexLocks.end(); ++it)
  {
    EXPECT_EQ(it->first.lower(), lockedSince);
    EXPECT_EQ(it->first.upper(), lockedTill);
    ConstraintsStub::VertexLockIntervalType lockedToSet = it->second;
    EXPECT_EQ(lockedToSet.size(), 1);
    EXPECT_EQ(*lockedToSet.begin(), defaultRunner);
  }
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, otherRunner, 0, lockedSince));
  EXPECT_FALSE(constraints.isVertexFreeForRunner(defaultVertex, otherRunner, lockedSince, lockedTill));
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, otherRunner, lockedTill));
}

TEST(Constraints, lock_vertex_with_non_overlaping_intervals_and_same_runner)
{
  const unsigned lockedSince1 = 3;
  const unsigned lockedTill1 = 11;
  const unsigned lockedSince2 = 23;
  const unsigned lockedTill2 = 27;
  DefaultGraphLoader loader;
  WeightedDiGraph graph = loader.getGraph();
  ConstraintsStub constraints(graph);
  EXPECT_TRUE(constraints.lockVertex(defaultVertex, defaultRunner, lockedSince1, lockedTill1));
  EXPECT_TRUE(constraints.lockVertex(defaultVertex, defaultRunner, lockedSince2, lockedTill2));
  const auto& vertexLocks = constraints.getVertexLocks(defaultVertex);
  auto numberOfIntervals = vertexLocks.iterative_size();
  EXPECT_EQ(numberOfIntervals, 2u);
  for (auto it = vertexLocks.begin(); it != vertexLocks.end(); ++it)
  {
    ConstraintsStub::VertexLockIntervalType lockedToSet = it->second;
    EXPECT_EQ(lockedToSet.size(), 1);
    EXPECT_EQ(*lockedToSet.begin(), defaultRunner);
  }
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, otherRunner, 0, lockedSince1));
  EXPECT_FALSE(constraints.isVertexFreeForRunner(defaultVertex, otherRunner, lockedSince1, lockedTill1));
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, otherRunner, lockedTill1, lockedSince2));
  EXPECT_FALSE(constraints.isVertexFreeForRunner(defaultVertex, otherRunner, lockedSince2, lockedTill2));
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, otherRunner, lockedTill2));
}

TEST(Constraints, lock_vertex_with_non_overlaping_intervals_and_different_runner)
{
  const unsigned lockedSince1 = 3;
  const unsigned lockedTill1 = 11;
  const unsigned lockedSince2 = 23;
  const unsigned lockedTill2 = 27;
  DefaultGraphLoader loader;
  WeightedDiGraph graph = loader.getGraph();
  ConstraintsStub constraints(graph);
  EXPECT_TRUE(constraints.lockVertex(defaultVertex, defaultRunner, lockedSince1, lockedTill1));
  EXPECT_TRUE(constraints.lockVertex(defaultVertex, otherRunner, lockedSince2, lockedTill2));
  const auto& vertexLocks = constraints.getVertexLocks(defaultVertex);
  auto numberOfIntervals = vertexLocks.iterative_size();
  EXPECT_EQ(numberOfIntervals, 2u);
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, otherRunner, 0, lockedSince1));
  EXPECT_FALSE(constraints.isVertexFreeForRunner(defaultVertex, otherRunner, lockedSince1, lockedTill1));
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, otherRunner, lockedTill1, lockedSince2));
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, otherRunner, lockedSince2, lockedTill2));
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, otherRunner, lockedTill2));
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, defaultRunner, 0, lockedSince1));
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, defaultRunner, lockedSince1, lockedTill1));
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, defaultRunner, lockedTill1, lockedSince2));
  EXPECT_FALSE(constraints.isVertexFreeForRunner(defaultVertex, defaultRunner, lockedSince2, lockedTill2));
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, defaultRunner, lockedTill2));
}

TEST(Constraints, lock_vertex_does_not_lock_any_other_vertex)
{
  DefaultGraphLoader loader;
  WeightedDiGraph graph = loader.getGraph();
  ConstraintsStub constraints(graph);
  EXPECT_TRUE(constraints.lockVertex(defaultVertex, defaultRunner));
  EXPECT_EQ(constraints.getVertexLock(otherVertex, 0), std::nullopt);
}

TEST(Constraints, locked_vertex_is_free_for_runner_its_locked_to)
{
  DefaultGraphLoader loader;
  WeightedDiGraph graph = loader.getGraph();
  ConstraintsStub constraints(graph);
  EXPECT_TRUE(constraints.lockVertex(defaultVertex, defaultRunner));
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, defaultRunner));
}

TEST(Constraints, locked_vertex_is_not_free_to_other_runner_then_it_was_locked_to)
{
  DefaultGraphLoader loader;
  WeightedDiGraph graph = loader.getGraph();
  ConstraintsStub constraints(graph);
  EXPECT_TRUE(constraints.lockVertex(defaultVertex, defaultRunner));
  EXPECT_FALSE(constraints.isVertexFreeForRunner(defaultVertex, otherRunner));
}

TEST(Constraints, unlock_vertex_releases_the_lock_of_given_runner_on_vertex)
{
  DefaultGraphLoader loader;
  WeightedDiGraph graph = loader.getGraph();
  ConstraintsStub constraints(graph);
  EXPECT_TRUE(constraints.lockVertex(defaultVertex, defaultRunner));
  constraints.unlockVertex(defaultVertex, defaultRunner);
  EXPECT_EQ(constraints.getVertexLock(defaultVertex, 0), std::nullopt);
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, defaultRunner));
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, otherRunner));
}

TEST(Constraints, unlock_vertex_does_not_release_the_lock_if_called_for_other_runner)
{
  DefaultGraphLoader loader;
  WeightedDiGraph graph = loader.getGraph();
  ConstraintsStub constraints(graph);
  EXPECT_TRUE(constraints.lockVertex(defaultVertex, defaultRunner));
  constraints.unlockVertex(defaultVertex, otherRunner);
  EXPECT_EQ(constraints.getVertexLock(defaultVertex, 0), defaultRunner);
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, defaultRunner));
  EXPECT_FALSE(constraints.isVertexFreeForRunner(defaultVertex, otherRunner));
}

TEST(Constraints, unlock_vertex_called_on_vertex_that_is_not_locked_preserves_its_state)
{
  DefaultGraphLoader loader;
  WeightedDiGraph graph = loader.getGraph();
  ConstraintsStub constraints(graph);
  constraints.unlockVertex(defaultVertex, defaultRunner);
  EXPECT_EQ(constraints.getVertexLock(defaultVertex, 0), std::nullopt);
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, defaultRunner));
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, otherRunner));
}

TEST(Constraints, unlock_vertex_releases_all_locks_of_given_runner_on_vertex_if_called_without_specified_interval)
{
  DefaultGraphLoader loader;
  WeightedDiGraph graph = loader.getGraph();
  ConstraintsStub constraints(graph);
  EXPECT_TRUE(constraints.lockVertex(defaultVertex, defaultRunner, 2, 3));
  EXPECT_TRUE(constraints.lockVertex(defaultVertex, defaultRunner, 5, 7));
  constraints.unlockVertex(defaultVertex, defaultRunner);
  EXPECT_EQ(constraints.getVertexLock(defaultVertex, 0), std::nullopt);
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, defaultRunner));
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, otherRunner));
}

TEST(Constraints, unlock_vertex_preserves_locks_of_other_runner)
{
  DefaultGraphLoader loader;
  WeightedDiGraph graph = loader.getGraph();
  ConstraintsStub constraints(graph);
  EXPECT_TRUE(constraints.lockVertex(defaultVertex, defaultRunner, 2, 3));
  EXPECT_TRUE(constraints.lockVertex(defaultVertex, otherRunner, 11, 13));
  EXPECT_TRUE(constraints.lockVertex(defaultVertex, defaultRunner, 5, 7));
  constraints.unlockVertex(defaultVertex, defaultRunner);
  EXPECT_EQ(constraints.getVertexLock(defaultVertex, 0), std::nullopt);
  EXPECT_EQ(constraints.getVertexLock(defaultVertex, 11), otherRunner);
  EXPECT_FALSE(constraints.isVertexFreeForRunner(defaultVertex, defaultRunner));
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, otherRunner));
}

TEST(Constraints, unlock_vertex_releases_whole_interval_if_called_with_same_interval)
{
  DefaultGraphLoader loader;
  WeightedDiGraph graph = loader.getGraph();
  ConstraintsStub constraints(graph);
  EXPECT_TRUE(constraints.lockVertex(defaultVertex, defaultRunner, 5, 7));
  constraints.unlockVertex(defaultVertex, defaultRunner, 5, 7);
  const auto& vertexLocks = constraints.getVertexLocks(defaultVertex);
  EXPECT_TRUE(vertexLocks.empty());
  auto numberOfIntervals = vertexLocks.iterative_size();
  EXPECT_EQ(numberOfIntervals, 0u);
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, defaultRunner));
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, otherRunner));
}

TEST(Constraints, unlock_vertex_releases_whole_interval_if_called_with_larger_interval)
{
  DefaultGraphLoader loader;
  WeightedDiGraph graph = loader.getGraph();
  ConstraintsStub constraints(graph);
  EXPECT_TRUE(constraints.lockVertex(defaultVertex, defaultRunner, 5, 7));
  constraints.unlockVertex(defaultVertex, defaultRunner, 3, 9);
  const auto& vertexLocks = constraints.getVertexLocks(defaultVertex);
  EXPECT_TRUE(vertexLocks.empty());
  auto numberOfIntervals = vertexLocks.iterative_size();
  EXPECT_EQ(numberOfIntervals, 0u);
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, defaultRunner));
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, otherRunner));
}

TEST(Constraints, unlock_vertex_releases_part_of_the_interval_if_called_with_overlaping_interval_after)
{
  DefaultGraphLoader loader;
  WeightedDiGraph graph = loader.getGraph();
  ConstraintsStub constraints(graph);
  EXPECT_TRUE(constraints.lockVertex(defaultVertex, defaultRunner, 3, 9));
  constraints.unlockVertex(defaultVertex, defaultRunner, 7, 9);
  const auto& vertexLocks = constraints.getVertexLocks(defaultVertex);
  auto numberOfIntervals = vertexLocks.iterative_size();
  EXPECT_EQ(numberOfIntervals, 1u);
  for (auto it = vertexLocks.begin(); it != vertexLocks.end(); ++it)
  {
    EXPECT_EQ(it->first.lower(), 3u);
    EXPECT_EQ(it->first.upper(), 7u);
    ConstraintsStub::VertexLockIntervalType lockedToSet = it->second;
    EXPECT_EQ(lockedToSet.size(), 1);
    EXPECT_EQ(*lockedToSet.begin(), defaultRunner);
  }
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, defaultRunner));
  EXPECT_FALSE(constraints.isVertexFreeForRunner(defaultVertex, otherRunner));
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, otherRunner, 0, 3));
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, otherRunner, 9));
}

TEST(Constraints, unlock_vertex_releases_part_of_the_interval_if_called_with_overlaping_interval_before)
{
  DefaultGraphLoader loader;
  WeightedDiGraph graph = loader.getGraph();
  ConstraintsStub constraints(graph);
  EXPECT_TRUE(constraints.lockVertex(defaultVertex, defaultRunner, 3, 9));
  constraints.unlockVertex(defaultVertex, defaultRunner, 2, 5);
  const auto& vertexLocks = constraints.getVertexLocks(defaultVertex);
  auto numberOfIntervals = vertexLocks.iterative_size();
  EXPECT_EQ(numberOfIntervals, 1u);
  for (auto it = vertexLocks.begin(); it != vertexLocks.end(); ++it)
  {
    EXPECT_EQ(it->first.lower(), 5u);
    EXPECT_EQ(it->first.upper(), 9u);
    ConstraintsStub::VertexLockIntervalType lockedToSet = it->second;
    EXPECT_EQ(lockedToSet.size(), 1);
    EXPECT_EQ(*lockedToSet.begin(), defaultRunner);
  }
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, defaultRunner));
  EXPECT_FALSE(constraints.isVertexFreeForRunner(defaultVertex, otherRunner));
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, otherRunner, 0, 5));
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, otherRunner, 9));
}

TEST(Constraints, unlock_vertex_releases_part_of_the_interval_if_called_with_overlaping_interval_inside)
{
  DefaultGraphLoader loader;
  WeightedDiGraph graph = loader.getGraph();
  ConstraintsStub constraints(graph);
  EXPECT_TRUE(constraints.lockVertex(defaultVertex, defaultRunner, 3, 9));
  constraints.unlockVertex(defaultVertex, defaultRunner, 5, 7);
  const auto& vertexLocks = constraints.getVertexLocks(defaultVertex);
  auto numberOfIntervals = vertexLocks.iterative_size();
  EXPECT_EQ(numberOfIntervals, 2u);
  for (auto it = vertexLocks.begin(); it != vertexLocks.end(); ++it)
  {
    ConstraintsStub::VertexLockIntervalType lockedToSet = it->second;
    EXPECT_EQ(lockedToSet.size(), 1);
    EXPECT_EQ(*lockedToSet.begin(), defaultRunner);
  }
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, defaultRunner));
  EXPECT_FALSE(constraints.isVertexFreeForRunner(defaultVertex, otherRunner));
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, otherRunner, 0, 3));
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, otherRunner, 5, 7));
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, otherRunner, 9));
}

TEST(Constraints, unlock_vertex_preserves_lock_if_called_with_disjoint_interval)
{
  DefaultGraphLoader loader;
  WeightedDiGraph graph = loader.getGraph();
  ConstraintsStub constraints(graph);
  EXPECT_TRUE(constraints.lockVertex(defaultVertex, defaultRunner, 3, 5));
  constraints.unlockVertex(defaultVertex, defaultRunner, 7, 9);
  const auto& vertexLocks = constraints.getVertexLocks(defaultVertex);
  auto numberOfIntervals = vertexLocks.iterative_size();
  EXPECT_EQ(numberOfIntervals, 1u);
  for (auto it = vertexLocks.begin(); it != vertexLocks.end(); ++it)
  {
    ConstraintsStub::VertexLockIntervalType lockedToSet = it->second;
    EXPECT_EQ(lockedToSet.size(), 1);
    EXPECT_EQ(*lockedToSet.begin(), defaultRunner);
  }
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, defaultRunner));
  EXPECT_FALSE(constraints.isVertexFreeForRunner(defaultVertex, otherRunner));
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, otherRunner, 0, 3));
  EXPECT_FALSE(constraints.isVertexFreeForRunner(defaultVertex, otherRunner, 3, 5));
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, otherRunner, 5));
}

TEST(Constraints, unlock_vertex_unlocks_the_vertex_interval)
{
  DefaultGraphLoader loader;
  WeightedDiGraph graph = loader.getGraph();
  ConstraintsStub constraints(graph);
  constraints.lockVertex(1, 7);
  auto vertexLocks = constraints.getVertexLocks(1);
  std::cout << vertexLocks << std::endl;
  constraints.unlockVertex(1, 7);
  vertexLocks = constraints.getVertexLocks(1);
  std::cout << vertexLocks << std::endl;
  EXPECT_TRUE(vertexLocks.empty());
  auto numberOfIntervals = vertexLocks.iterative_size();
  EXPECT_EQ(numberOfIntervals, 0u);
}

TEST(Constraints, unlock_vertex_does_not_unlock_any_other_vertex)
{
  DefaultGraphLoader loader;
  WeightedDiGraph graph = loader.getGraph();
  ConstraintsStub constraints(graph);
  for (size_t vertex = 0; vertex < graph.m_vertices.size(); ++vertex)
  {
    constraints.lockVertex(vertex, 7);
  }
  constraints.unlockVertex(1, 7);
  EXPECT_EQ(constraints.getVertexLock(0, 0), 7);
  EXPECT_EQ(constraints.getVertexLock(2, 0), 7);
  EXPECT_EQ(constraints.getVertexLock(3, 0), 7);
}

TEST(Constraints, unlocked_vertex_is_free_for_any_runner)
{
  DefaultGraphLoader loader;
  WeightedDiGraph graph = loader.getGraph();
  ConstraintsStub constraints(graph);
  constraints.lockVertex(1, 7);
  constraints.unlockVertex(1, 7);
  const size_t numberOfRunners = 10;
  for (auto runner = 0; runner < numberOfRunners; ++runner)
  {
    EXPECT_TRUE(constraints.isVertexFreeForRunner(1, runner));
  }
}

TEST(Constraints, is_vertex_free_for_runner_returns_false_if_other_runner_locked_part_of_requested_interval)
{
  DefaultGraphLoader loader;
  WeightedDiGraph graph = loader.getGraph();
  ConstraintsStub constraints(graph);
  constraints.lockVertex(defaultVertex, defaultRunner, 5, 7);
  constraints.lockVertex(defaultVertex, otherRunner, 7, 9);
  EXPECT_FALSE(constraints.isVertexFreeForRunner(defaultVertex, defaultRunner));
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, defaultRunner, 0, 4));
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, otherRunner, 0, 4));
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, defaultRunner, 0, 5));
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, otherRunner, 0, 5));
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, defaultRunner, 0, 6));
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, defaultRunner, 0, 7));
  EXPECT_FALSE(constraints.isVertexFreeForRunner(defaultVertex, defaultRunner, 0, 8));
  EXPECT_FALSE(constraints.isVertexFreeForRunner(defaultVertex, defaultRunner, 5, 8));
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, defaultRunner, 5, 6));
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, defaultRunner, 5, 7));
  EXPECT_TRUE(constraints.isVertexFreeForRunner(defaultVertex, defaultRunner, 6, 7));
  EXPECT_FALSE(constraints.isVertexFreeForRunner(defaultVertex, defaultRunner, 6, 8));
}
