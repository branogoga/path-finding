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
    void finishRunnerJobs();
    void moveRunners();

    bool isJobAssignedToRunner(unsigned runnerId) const;
    void assignNextJobToRunner(unsigned runnerId);
    void finishRunnerJob(unsigned runnerId);

    bool isVertexFreeForRunner(const Vertex& vertex, unsigned runnerId) const;
    bool lockVertex(const Vertex& vertex, unsigned runnerId);
    void unlockVertex(const Vertex& vertex);

private:
    std::vector<Runner> runners;
    WeightedDiGraph graph;
    std::vector<std::optional<unsigned>> vertexLocks;
    // TODO: edge locks

    std::vector<JobRequest> newJobRequests;
    std::vector<std::optional<JobRequest>> jobAssignments;
    std::vector<JobRequest> finishedJobRequests;

    unsigned time;
};