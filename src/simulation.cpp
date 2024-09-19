#include "simulation.h"

#include <iostream>

Simulation::Simulation(
    const std::vector<JobRequest>& jobRequests,
    const WeightedDiGraph& graph,
    unsigned numberOfRunners,
    MultiAgentShortestPathCalculator shortestPathStrategy)
    : newJobRequests(
        jobRequests.rbegin(), jobRequests.rend())  // Revert the list, so that we can quickly pop first job from back
    , jobAssignments(numberOfRunners, std::nullopt)
    , graph(graph)
    , time(0)
    , constraints(graph)
    , someRunnerMovedInLastStep(true)
    , shortestPathStrategy(shortestPathStrategy)
{
  // Create empty runners at initial position
  for (unsigned i = 0; i < numberOfRunners; ++i)
  {
    runners.emplace_back(graph, 0);
  }
}

void Simulation::assignNewJobsToRunners()
{
  for (unsigned runnerIndex = 0; runnerIndex < runners.size(); ++runnerIndex)
  {
    if (!isJobAssignedToRunner(runnerIndex))
    {
      assignNextJobToRunner(runnerIndex);
    }
  }
}

bool Simulation::isJobAssignedToRunner(unsigned runnerId) const
{
  return jobAssignments[runnerId] != std::nullopt;
}

void Simulation::assignNextJobToRunner(unsigned runnerId)
{
  if (isJobAssignedToRunner(runnerId))
  {
    std::ostringstream message;
    message << "Failed to assign new job to runner '" << runnerId << "': Runner has already assigned other job.";
    throw std::runtime_error(message.str());
  }

  if (!newJobRequests.empty())
  {
    auto jobRequest = newJobRequests.back();
    newJobRequests.pop_back();
    jobAssignments[runnerId] = jobRequest;
    const auto& path = shortestPathStrategy(graph, jobRequest.startVertex, jobRequest.endVertex, constraints, runnerId);
    constraints.unlockVertex(runners[runnerId].getLastVisitedVertex(), runnerId, time /* +1 */);
    runners[runnerId].travel(path, true);
    lockPathForRunner(runnerId, path);
    // unsigned time_since_start = 0;
    // bool isPathFree = true;
    // for (auto vertex : path)
    // {
    //   const auto startTime = time + time_since_start;
    //   const auto endTime = time + time_since_start + 1;
    //   const bool isVertexFree = constraints.isVertexFreeForRunner(vertex, runnerId, startTime, endTime);
    //   isPathFree &= isVertexFree;
    //   ++time_since_start;
    //   if (!isVertexFree)
    //   {
    //     std::ostringstream message;
    //     message << "Failed to assign new job to runner '" << runnerId << "': Vertex "
    //             << runners[runnerId].getLastVisitedVertex() << " is already locked to runner "
    //             << *constraints.getVertexLock(runners[runnerId].getLastVisitedVertex(), time);
    //   }
    // }
    // if (isPathFree)
    // {
    //   time_since_start = 0;
    //   for (auto vertex : path)
    //   {
    //     const auto startTime = time + time_since_start;
    //     const auto endTime = time + time_since_start + 1;
    //     constraints.lockVertex(vertex, runnerId, startTime, endTime);
    //     std::cout << "Locking vertex " << vertex << " to runner " << runnerId << " since " << startTime << " till
    //     "
    //               << endTime << std::endl;
    //     ++time_since_start;
    //   }
    // }
    // else
    // {
    //   std::ostringstream message;
    //   message << "Unable to lock path for runner #" << runnerId << ". Some vertices are locked.";
    //   throw std::runtime_error(message.str());
    // }
    std::cout << time << " - Runner " << runnerId << " - assigned new job " << jobRequest.startVertex << " "
              << graph[jobRequest.startVertex].position << " > " << jobRequest.endVertex << " "
              << graph[jobRequest.endVertex].position << ", path=[" << path << "]" << std::endl;
  }
}

void Simulation::finishRunnerJob(unsigned runnerId)
{
  if (!isJobAssignedToRunner(runnerId))
  {
    std::ostringstream message;
    message << "Failed to finish job on runner '" << runnerId << "': There is no job assigned to this runner.";
    throw std::runtime_error(message.str());
  }

  const auto jobRequest = jobAssignments[runnerId];
  jobAssignments[runnerId] = std::nullopt;
  finishedJobRequests.push_back(*jobRequest);
  std::cout << time << " - Runner " << runnerId << " - finished job " << jobRequest->startVertex << " "
            << graph[jobRequest->startVertex].position << " > " << jobRequest->endVertex << " "
            << graph[jobRequest->endVertex].position << std::endl;
}

void Simulation::finishRunnerJobs()
{
  for (unsigned runnerId = 0; runnerId < runners.size(); ++runnerId)
  {
    if (runners[runnerId].isInDestination() && isJobAssignedToRunner(runnerId))
    {
      finishRunnerJob(runnerId);
    }
  }
}

void Simulation::moveRunners()
{
  for (auto i = 0; i < graph.m_vertices.size(); ++i)
  {
    auto lock = constraints.getVertexLock(i, time);
    if (lock)
    {
      std::cout << time << " - Vertex " << i << " is locked to runner " << (lock ? *lock : -1) << std::endl;
    }
  }

  someRunnerMovedInLastStep = false;
  for (unsigned runnerId = 0; runnerId < runners.size(); ++runnerId)
  {
    auto& runner = runners[runnerId];
    if (constraints.lockVertex(runner.getNextVertex(), runnerId, time, time + 1))
    {
      const auto previousVertex = runner.getLastVisitedVertex();
      runner.advance();
      if (previousVertex != runner.getLastVisitedVertex())
      {
        someRunnerMovedInLastStep = true;
      }
      // if (previousVertex != runner.getLastVisitedVertex())
      // {
      //   constraints.unlockVertex(previousVertex, runnerId);
      // }
      if (previousVertex != runner.getLastVisitedVertex())
      {
        std::cout << time << " - Runner " << runnerId << " moved from vertex " << previousVertex << " "
                  << graph[previousVertex].position << " to vertex " << runner.getLastVisitedVertex() << " "
                  << runner.getPosition() << std::endl;
      }
      else
      {
        std::cout << time << " - Runner " << runnerId << " waits at vertex " << previousVertex << " "
                  << graph[previousVertex].position << std::endl;
      }
    }
    else
    {
      // TODO: Is the vertex where we stay free at the next moment ???
      std::cout << time << " - Runner " << runnerId << " stays at vertex " << runner.getLastVisitedVertex() << " "
                << runner.getPosition() << ". Next vertex " << runner.getNextVertex() << " is locked to runner"
                << *constraints.getVertexLock(runner.getNextVertex(), time) << std::endl;
    }
  }
}

void Simulation::lockPathForRunner(RunnerId runnerId, const Path& path)
{
  unsigned time_since_start = 0;
  bool isPathFree = true;
  for (auto vertex : path)
  {
    const auto startTime = time + time_since_start;
    const auto endTime = time + time_since_start + 1;
    const bool isVertexFree = constraints.isVertexFreeForRunner(vertex, runnerId, startTime, endTime);
    isPathFree &= isVertexFree;
    ++time_since_start;
    if (!isVertexFree)
    {
      std::ostringstream message;
      message << "Failed to assign new job to runner '" << runnerId << "': Vertex "
              << runners[runnerId].getLastVisitedVertex() << " is already locked to runner "
              << *constraints.getVertexLock(runners[runnerId].getLastVisitedVertex(), time);
    }
  }
  if (isPathFree)
  {
    time_since_start = 0;
    for (auto vertex : path)
    {
      const auto startTime = time + time_since_start;
      const auto endTime = time + time_since_start + 1;
      constraints.lockVertex(vertex, runnerId, startTime, endTime);
      std::cout << "Locking vertex " << vertex << " to runner " << runnerId << " since " << startTime << " till "
                << endTime << std::endl;
      ++time_since_start;
    }
  }
  else
  {
    std::ostringstream message;
    message << "Unable to lock path for runner #" << runnerId << ". Some vertices are locked.";
    throw std::runtime_error(message.str());
  }
}

void Simulation::advance()
{
  assignNewJobsToRunners();
  moveRunners();
  finishRunnerJobs();
  ++time;
}

bool Simulation::areAllRunnersFinished() const
{
  bool areAllRunnersFinished = true;
  for (unsigned runnerId = 0; runnerId < runners.size(); ++runnerId)
  {
    areAllRunnersFinished &= !isJobAssignedToRunner(runnerId);
    areAllRunnersFinished &= runners[runnerId].isInDestination();
  }
  return areAllRunnersFinished;
}

bool Simulation::isFinished() const
{
  return (newJobRequests.empty() && areAllRunnersFinished()) /*|| !someRunnerMovedInLastStep*/;
}

bool Simulation::isDeadlock() const
{
  return !areAllRunnersFinished() && !someRunnerMovedInLastStep;
}

unsigned Simulation::getTime() const
{
  return time;
}

const std::vector<JobRequest>& Simulation::getNewJobRequests() const
{
  return newJobRequests;
}

const std::vector<std::optional<JobRequest>>& Simulation::getJobAssignments() const
{
  return jobAssignments;
}

const std::vector<JobRequest>& Simulation::getFinishedJobRequests() const
{
  return finishedJobRequests;
}

const std::vector<Runner>& Simulation::getRunners() const
{
  return runners;
}
