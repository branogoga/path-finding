#include "simulation.h"

#include <gtest/gtest.h>

#include <boost/graph/adjacency_list.hpp>

const unsigned defaultNumberOfVertices = 9;

WeightedDiGraph createGraph(unsigned numberOfVertices = defaultNumberOfVertices)
{
  WeightedDiGraph graph(numberOfVertices);
  for (unsigned i = 0; i < numberOfVertices; ++i)
  {
    add_edge(i, i + 1, 1.0f, graph);
    add_edge(i + 1, i, 1.0f, graph);
  }
  return graph;
}

class SimulationStub : public Simulation
{
 public:
  friend class SimulationTest;
  SimulationStub() = delete;
  SimulationStub(const std::vector<JobRequest>& jobRequests, const WeightedDiGraph& graph, unsigned numberOfRunners = 0)
      : Simulation(jobRequests, graph, numberOfRunners)
  {
  }

  bool isJobAssignedToRunner(unsigned runnerId) const
  {
    return Simulation::isJobAssignedToRunner(runnerId);
  }

  void assignNextJobToRunner(unsigned runnerId)
  {
    Simulation::assignNextJobToRunner(runnerId);
  };

  void finishRunnerJob(unsigned runnerId)
  {
    Simulation::finishRunnerJob(runnerId);
  }

  void moveRunners()
  {
    Simulation::moveRunners();
  }

  bool isVertexFreeForRunner(const Vertex& vertex, unsigned runnerId) const
  {
    return constraints.isVertexFreeForRunner(vertex, runnerId);
  }

  bool lockVertex(const Vertex& vertex, unsigned runnerId)
  {
    return constraints.lockVertex(vertex, runnerId);
  }

  void unlockVertex(const Vertex& vertex, unsigned runnerId)
  {
    constraints.unlockVertex(vertex, runnerId);
  }
};

TEST(SimulationTest, creates_no_runners)
{
  WeightedDiGraph graph = createGraph();
  std::vector<JobRequest> jobRequests{JobRequest{0, 1}};
  const unsigned numberOfRunners = 0;
  SimulationStub simulation(jobRequests, graph, numberOfRunners);
  EXPECT_TRUE(simulation.getRunners().empty());
}

TEST(SimulationTest, creates_requested_number_of_runners)
{
  WeightedDiGraph graph = createGraph();
  std::vector<JobRequest> jobRequests{JobRequest{0, 1}};
  const unsigned numberOfRunners = 3;
  SimulationStub simulation(jobRequests, graph, numberOfRunners);
  EXPECT_EQ(simulation.getRunners().size(), numberOfRunners);
}

TEST(SimulationTest, is_not_finished_when_initialized_with_some_job_requests)
{
  WeightedDiGraph graph = createGraph();
  std::vector<JobRequest> jobRequests{JobRequest{0, 1}};
  const unsigned numberOfRunners = 1;
  SimulationStub simulation(jobRequests, graph, numberOfRunners);
  EXPECT_FALSE(simulation.isFinished());
}

TEST(SimulationTest, is_finished_when_initialized_without_job_requests)
{
  WeightedDiGraph graph = createGraph();
  std::vector<JobRequest> jobRequests{};
  const unsigned numberOfRunners = 1;
  SimulationStub simulation(jobRequests, graph, numberOfRunners);
  EXPECT_TRUE(simulation.isFinished());
}

TEST(SimulationTest, time_is_zero_before_start)
{
  WeightedDiGraph graph = createGraph();
  std::vector<JobRequest> jobRequests{JobRequest{0, 1}};
  const unsigned numberOfRunners = 1;
  SimulationStub simulation(jobRequests, graph, numberOfRunners);
  EXPECT_EQ(simulation.getTime(), 0u);
}

TEST(SimulationTest, increments_time_after_step)
{
  WeightedDiGraph graph = createGraph();
  std::vector<JobRequest> jobRequests{JobRequest{0, 1}};
  const unsigned numberOfRunners = 1;
  SimulationStub simulation(jobRequests, graph, numberOfRunners);
  simulation.advance();
  EXPECT_EQ(simulation.getTime(), 1u);
}

TEST(SimulationTest, initial_job_assignments)
{
  WeightedDiGraph graph = createGraph();
  std::vector<JobRequest> jobRequests{JobRequest{1, 2}};
  const unsigned numberOfRunners = 2;

  SimulationStub simulation(jobRequests, graph, numberOfRunners);

  EXPECT_FALSE(simulation.isJobAssignedToRunner(0));
  EXPECT_FALSE(simulation.isJobAssignedToRunner(1));
  EXPECT_EQ(simulation.getNewJobRequests().size(), 1);
  EXPECT_EQ(simulation.getJobAssignments().size(), 2);
  EXPECT_EQ(simulation.getJobAssignments()[0], std::nullopt);
  EXPECT_EQ(simulation.getJobAssignments()[1], std::nullopt);
  EXPECT_TRUE(simulation.getFinishedJobRequests().empty());
  EXPECT_FALSE(simulation.isFinished());
}

TEST(SimulationTest, assigns_job_to_runner)
{
  WeightedDiGraph graph = createGraph();
  JobRequest jobRequest{1, 2};
  std::vector<JobRequest> jobRequests{jobRequest};
  const unsigned numberOfRunners = 2;

  SimulationStub simulation(jobRequests, graph, numberOfRunners);
  simulation.assignNextJobToRunner(1);

  EXPECT_FALSE(simulation.isJobAssignedToRunner(0));
  EXPECT_TRUE(simulation.isJobAssignedToRunner(1));
  EXPECT_TRUE(simulation.getNewJobRequests().empty());
  EXPECT_EQ(simulation.getJobAssignments().size(), 2);
  EXPECT_EQ(simulation.getJobAssignments()[0], std::nullopt);
  EXPECT_EQ(simulation.getJobAssignments()[1]->startVertex, jobRequest.startVertex);
  EXPECT_EQ(simulation.getJobAssignments()[1]->endVertex, jobRequest.endVertex);
  EXPECT_TRUE(simulation.getFinishedJobRequests().empty());
  EXPECT_EQ(simulation.getRunners()[1].getLastVisitedVertex(), jobRequest.startVertex);
  EXPECT_EQ(simulation.getRunners()[1].getDestinationVertex(), jobRequest.endVertex);
  EXPECT_FALSE(simulation.isFinished());
}

TEST(SimulationTest, moveRunners)
{
  WeightedDiGraph graph = createGraph();
  JobRequest jobRequest{1, 2};
  std::vector<JobRequest> jobRequests{jobRequest};
  const unsigned numberOfRunners = 2;

  SimulationStub simulation(jobRequests, graph, numberOfRunners);
  simulation.assignNextJobToRunner(1);
  simulation.moveRunners();

  EXPECT_FALSE(simulation.isJobAssignedToRunner(0));
  EXPECT_TRUE(simulation.isJobAssignedToRunner(1));
  EXPECT_TRUE(simulation.getNewJobRequests().empty());
  EXPECT_EQ(simulation.getJobAssignments().size(), 2);
  EXPECT_EQ(simulation.getJobAssignments()[0], std::nullopt);
  EXPECT_EQ(simulation.getJobAssignments()[1]->startVertex, jobRequest.startVertex);
  EXPECT_EQ(simulation.getJobAssignments()[1]->endVertex, jobRequest.endVertex);
  EXPECT_TRUE(simulation.getFinishedJobRequests().empty());
  EXPECT_EQ(simulation.getRunners()[1].getLastVisitedVertex(), jobRequest.endVertex);
  EXPECT_EQ(simulation.getRunners()[1].getDestinationVertex(), jobRequest.endVertex);
  EXPECT_TRUE(simulation.getRunners()[1].isInDestination());
  EXPECT_FALSE(simulation.isFinished());
}

TEST(SimulationTest, finishRunnerJob)
{
  WeightedDiGraph graph = createGraph();
  JobRequest jobRequest{1, 2};
  std::vector<JobRequest> jobRequests{jobRequest};
  const unsigned numberOfRunners = 2;

  SimulationStub simulation(jobRequests, graph, numberOfRunners);
  simulation.assignNextJobToRunner(1);
  simulation.moveRunners();
  simulation.finishRunnerJob(1);

  EXPECT_FALSE(simulation.isJobAssignedToRunner(0));
  EXPECT_FALSE(simulation.isJobAssignedToRunner(1));
  EXPECT_TRUE(simulation.getNewJobRequests().empty());
  EXPECT_EQ(simulation.getJobAssignments().size(), 2);
  EXPECT_EQ(simulation.getJobAssignments()[0], std::nullopt);
  EXPECT_EQ(simulation.getJobAssignments()[1], std::nullopt);
  EXPECT_EQ(simulation.getFinishedJobRequests().size(), 1);
  EXPECT_EQ(simulation.getRunners()[1].getLastVisitedVertex(), jobRequest.endVertex);
  EXPECT_EQ(simulation.getRunners()[1].getDestinationVertex(), jobRequest.endVertex);
  EXPECT_TRUE(simulation.isFinished());
}

TEST(SimulationTest, no_lock_on_vertices_initialy)
{
  WeightedDiGraph graph = createGraph();
  JobRequest jobRequest{1, 2};
  std::vector<JobRequest> jobRequests{jobRequest};
  const unsigned numberOfRunners = 2;

  SimulationStub simulation(jobRequests, graph, numberOfRunners);
  for (unsigned vertex = 0; vertex < graph.m_vertices.size(); ++vertex)
  {
    EXPECT_TRUE(simulation.isVertexFreeForRunner(vertex, 0));
    EXPECT_TRUE(simulation.isVertexFreeForRunner(vertex, 1));
  }
}

TEST(SimulationTest, locks_vertex_for_runner)
{
  WeightedDiGraph graph = createGraph();
  JobRequest jobRequest{1, 2};
  std::vector<JobRequest> jobRequests{jobRequest};
  const unsigned numberOfRunners = 2;

  SimulationStub simulation(jobRequests, graph, numberOfRunners);
  simulation.lockVertex(0, 1);
  EXPECT_FALSE(simulation.isVertexFreeForRunner(0, 0));
  EXPECT_TRUE(simulation.isVertexFreeForRunner(0, 1));
  for (unsigned vertex = 1; vertex < graph.m_vertices.size(); ++vertex)
  {
    EXPECT_TRUE(simulation.isVertexFreeForRunner(vertex, 0));
    EXPECT_TRUE(simulation.isVertexFreeForRunner(vertex, 1));
  }
}

TEST(SimulationTest, does_not_lock_vertex_already_locked_for_other_runner)
{
  WeightedDiGraph graph = createGraph();
  JobRequest jobRequest{1, 2};
  std::vector<JobRequest> jobRequests{jobRequest};
  const unsigned numberOfRunners = 2;

  SimulationStub simulation(jobRequests, graph, numberOfRunners);
  EXPECT_TRUE(simulation.lockVertex(0, 1));
  EXPECT_FALSE(simulation.lockVertex(0, 0));
}

TEST(SimulationTest, does_not_throw_if_trying_to_lock_vertex_locked_for_same_runner)
{
  WeightedDiGraph graph = createGraph();
  JobRequest jobRequest{1, 2};
  std::vector<JobRequest> jobRequests{jobRequest};
  const unsigned numberOfRunners = 2;

  SimulationStub simulation(jobRequests, graph, numberOfRunners);
  EXPECT_TRUE(simulation.lockVertex(0, 1));
  EXPECT_TRUE(simulation.lockVertex(0, 1));
}

TEST(SimulationTest, unlocks_vertex)
{
  WeightedDiGraph graph = createGraph();
  JobRequest jobRequest{1, 2};
  std::vector<JobRequest> jobRequests{jobRequest};
  const unsigned numberOfRunners = 2;

  SimulationStub simulation(jobRequests, graph, numberOfRunners);
  simulation.lockVertex(0, 1);
  simulation.unlockVertex(0, 1);
  for (unsigned vertex = 0; vertex < graph.m_vertices.size(); ++vertex)
  {
    EXPECT_TRUE(simulation.isVertexFreeForRunner(vertex, 0));
    EXPECT_TRUE(simulation.isVertexFreeForRunner(vertex, 1));
  }
}
