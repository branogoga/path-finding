#pragma once

#include "graph.h"
#include "runner.h"
#include "scenario.h"

class Simulation {
public:
    Simulation() = delete;
    Simulation(std::vector<JobRequest> jobRequests, unsigned numberOfRunners = 0);

    void advance();

    std::vector<Runner> runners;
    WeightedDiGraph graph;

    std::vector<JobRequest> newJobRequests;
    std::vector<JobRequest> finishedJobRequests;
};