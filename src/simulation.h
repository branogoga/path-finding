#pragma once

#include "constraints.h"
#include "graph.h"
#include "path-finding.h"
#include "runner.h"
#include "scenario.h"

class Simulation
{
 public:
  Simulation() = delete;
  Simulation(
      const std::vector<JobRequest> &jobRequests,
      const WeightedDiGraph &graph,
      unsigned numberOfRunners = 0,
      ShortestPathCalculator shortestPathStrategy = a_star_shortest_path);

  void advance();

  bool isFinished() const;
  bool isDeadlock() const;
  unsigned getTime() const;

  const std::vector<JobRequest> &getNewJobRequests() const;
  const std::vector<std::optional<JobRequest>> &getJobAssignments() const;
  const std::vector<JobRequest> &getFinishedJobRequests() const;
  const std::vector<Runner> &getRunners() const;

 protected:
  void assignNewJobsToRunners();
  void finishRunnerJobs();
  void moveRunners();

  bool isJobAssignedToRunner(RunnerId runnerId) const;
  void assignNextJobToRunner(RunnerId runnerId);
  void finishRunnerJob(RunnerId runnerId);

  bool areAllRunnersFinished() const;

  Constraints constraints;

 private:
  std::vector<Runner> runners;
  WeightedDiGraph graph;

  std::vector<JobRequest> newJobRequests;
  std::vector<std::optional<JobRequest>> jobAssignments;
  std::vector<JobRequest> finishedJobRequests;

  unsigned time;
  bool someRunnerMovedInLastStep;
  ShortestPathCalculator shortestPathStrategy;
};