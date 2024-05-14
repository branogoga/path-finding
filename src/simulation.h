#pragma once

#include "graph.h"
#include "runner.h"
#include "scenario.h"

class Simulation {
public:
    Simulation() = delete;
    Simulation(const std::vector<JobRequest>& jobRequests, const WeightedDiGraph& graph, unsigned numberOfRunners = 0);

    void advance();

    bool isFinished() const;
    unsigned getTime() const;

protected:
    void assignNewJobsToRunners();

    bool isJobAssignedToRunner(unsigned runnerId) const;
    void assignNextJobToRunner(unsigned runnerId);
    void finishRunnerJob(unsigned runnerId);

private:
    std::vector<Runner> runners;
    WeightedDiGraph graph;

    std::vector<JobRequest> newJobRequests;
    std::vector<std::optional<JobRequest>> jobAssignments;
    std::vector<JobRequest> finishedJobRequests;

    unsigned time;
};