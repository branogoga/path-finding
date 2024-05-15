#ifndef PATHFINDING_SCENARIO_H
#define PATHFINDING_SCENARIO_H

#include <string>
#include <vector>

#include "graph.h"

class JobRequest
{
 public:
  JobRequest() = delete;
  JobRequest(unsigned start, unsigned end);

  unsigned startVertex;
  unsigned endVertex;
};

class ScenarioLoader
{
 public:
  virtual WeightedDiGraph getGraph() const = 0;
  virtual std::vector<JobRequest> getjobRequests() const = 0;
};

class DefaultScenarioLoader : public ScenarioLoader
{
 public:
  WeightedDiGraph getGraph() const override;
  std::vector<JobRequest> getjobRequests() const override;
};

class FileScenarioLoader : public ScenarioLoader
{
 public:
  FileScenarioLoader(const std::string& filename);

  WeightedDiGraph getGraph() const override;
  std::vector<JobRequest> getjobRequests() const override;

 private:
  JobRequest parseJobRequest(const std::string& line, const std::string& filename);

  std::vector<JobRequest> jobRequests;
  std::unique_ptr<MapGraphLoader> graphLoader;
};

#endif PATHFINDING_SCENARIO_H