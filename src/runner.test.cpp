#include "runner.h"

#include <gtest/gtest.h>

#include <filesystem>

TEST(Runner, assigns_the_unique_id)
{
  DefaultGraphLoader graphLoader;
  const auto graph = graphLoader.getGraph();

  Runner runner1(graph, 0);
  ASSERT_EQ(runner1.getId(), 1);

  Runner runner2(graph, 1);
  ASSERT_EQ(runner2.getId(), 2);

  Runner runner3(graph, 2);
  ASSERT_EQ(runner3.getId(), 3);
}

TEST(Runner, assigns_initial_position)
{
  DefaultGraphLoader graphLoader;
  const auto graph = graphLoader.getGraph();

  const Vertex initialVertex = 1;
  Runner runner(graph, initialVertex);
  ASSERT_EQ(runner.getLastVisitedVertex(), initialVertex);
  ASSERT_EQ(runner.getDestinationVertex(), initialVertex);
  ASSERT_EQ(runner.getPosition(), graph.m_vertices[initialVertex].m_property.position);
  ASSERT_EQ(runner.getPath(), Path());
  ASSERT_FALSE(runner.isTraveling());
  ASSERT_TRUE(runner.isInDestination());
}

TEST(Runner, assignment_of_trajectory_updates_destination_vertex)
{
  DefaultGraphLoader graphLoader;
  const auto graph = graphLoader.getGraph();

  const Vertex initialVertex = 0;
  Runner runner(graph, initialVertex);
  runner.travel({0, 1, 2, 3});
  ASSERT_EQ(runner.getLastVisitedVertex(), initialVertex);
  ASSERT_EQ(runner.getDestinationVertex(), 3);
  ASSERT_EQ(runner.getPosition(), graph.m_vertices[initialVertex].m_property.position);
  ASSERT_EQ(runner.getPath(), Path({0, 1, 2, 3}));
  ASSERT_TRUE(runner.isTraveling());
  ASSERT_FALSE(runner.isInDestination());
}

// TODO: travel with empty path
// TODO: travel with single vertex path
// TODO: travel while already traveling

TEST(Runner, throws_exception_if_runner_is_asked_to_travel_from_different_position_to_where_it_is)
{
  DefaultGraphLoader graphLoader;
  const auto graph = graphLoader.getGraph();

  const Vertex initialVertex = 0;
  Runner runner(graph, initialVertex);
  ASSERT_THROW(runner.travel({2, 3}), std::exception);
}

TEST(
    Runner,
    teleports_runner_to_start_of_new_path_if_runner_is_asked_to_travel_from_different_position_to_where_it_is_and_explicitly_allowed)
{
  DefaultGraphLoader graphLoader;
  const auto graph = graphLoader.getGraph();

  const Vertex initialVertex = 0;
  Runner runner(graph, initialVertex);
  runner.travel({2, 3}, true);
  ASSERT_EQ(runner.getLastVisitedVertex(), 2);
  ASSERT_EQ(runner.getDestinationVertex(), 3);
  ASSERT_EQ(runner.getPosition(), graph.m_vertices[2].m_property.position);
  ASSERT_EQ(runner.getPath(), Path({2, 3}));
  ASSERT_TRUE(runner.isTraveling());
  ASSERT_FALSE(runner.isInDestination());
}

// TODO: implement and test advance() to step over the path

TEST(Runner, advances_position_of_the_runer_to_next_vertex)
{
  DefaultGraphLoader graphLoader;
  const auto graph = graphLoader.getGraph();

  const Vertex initialVertex = 0;
  Runner runner(graph, initialVertex);
  runner.travel({0, 1, 2, 3});
  runner.advance();
  ASSERT_EQ(runner.getLastVisitedVertex(), 1);
  ASSERT_EQ(runner.getDestinationVertex(), 3);
  ASSERT_EQ(runner.getPosition(), graph.m_vertices[1].m_property.position);
  ASSERT_EQ(runner.getPath(), Path({0, 1, 2, 3}));
  ASSERT_TRUE(runner.isTraveling());
  ASSERT_FALSE(runner.isInDestination());
}

TEST(Runner, finishes_traveling_when_reached_destination)
{
  DefaultGraphLoader graphLoader;
  const auto graph = graphLoader.getGraph();

  const Vertex initialVertex = 0;
  Runner runner(graph, initialVertex);
  runner.travel({0, 1});
  runner.advance();
  ASSERT_EQ(runner.getLastVisitedVertex(), 1);
  ASSERT_EQ(runner.getDestinationVertex(), 1);
  ASSERT_EQ(runner.getPosition(), graph.m_vertices[1].m_property.position);
  ASSERT_EQ(runner.getPath(), Path({0, 1}));
  ASSERT_FALSE(runner.isTraveling());
  ASSERT_TRUE(runner.isInDestination());
}

TEST(Runner, advance_does_not_change_state_of_runner_if_already_reached_the_destination)
{
  DefaultGraphLoader graphLoader;
  const auto graph = graphLoader.getGraph();

  const Vertex initialVertex = 0;
  Runner runner(graph, initialVertex);
  runner.travel({0, 1});
  runner.advance();
  ASSERT_EQ(runner.getLastVisitedVertex(), 1);
  ASSERT_EQ(runner.getDestinationVertex(), 1);
  ASSERT_EQ(runner.getPosition(), graph.m_vertices[1].m_property.position);
  ASSERT_EQ(runner.getPath(), Path({0, 1}));
  ASSERT_FALSE(runner.isTraveling());
  ASSERT_TRUE(runner.isInDestination());

  runner.advance();
  ASSERT_EQ(runner.getLastVisitedVertex(), 1);
  ASSERT_EQ(runner.getDestinationVertex(), 1);
  ASSERT_EQ(runner.getPosition(), graph.m_vertices[1].m_property.position);
  ASSERT_EQ(runner.getPath(), Path({0, 1}));
  ASSERT_FALSE(runner.isTraveling());
  ASSERT_TRUE(runner.isInDestination());
}
