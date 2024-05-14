#include "simulation.h"

#include <iostream>

Simulation::Simulation(const std::vector<JobRequest>& jobRequests, const WeightedDiGraph& graph, unsigned numberOfRunners) 
: newJobRequests(jobRequests.rbegin(), jobRequests.rend())  // Revert the list, so that we can quickly pop first job from back
, jobAssignments(numberOfRunners, std::nullopt)
, graph(graph)
, time(0) {
    // Create empty runners at initial position
    for(unsigned i = 0; i < numberOfRunners; ++i) {
        runners.emplace_back(graph, 0);
    }

    assignNewJobsToRunners();
}

void Simulation::assignNewJobsToRunners() {
    for(unsigned runnerIndex = 0; runnerIndex < runners.size(); ++runnerIndex) {
        if(!isJobAssignedToRunner(runnerIndex)) {
            assignNextJobToRunner(runnerIndex);
        }
    }
}


bool Simulation::isJobAssignedToRunner(unsigned runnerId) const {
    return jobAssignments[runnerId] != std::nullopt;
}

void Simulation::assignNextJobToRunner(unsigned runnerId) {   
    if(isJobAssignedToRunner(runnerId)) {
        std::ostringstream message;
        message << "Failed to assign new job to runner '" << runnerId << "': Runner has already assigned other job.";
        throw std::runtime_error(message.str());
    }

    if(!newJobRequests.empty()) {
        auto jobRequest = newJobRequests.back();
        newJobRequests.pop_back();
        jobAssignments[runnerId] = jobRequest;
        const auto& path = shortest_path(graph, jobRequest.startVertex, jobRequest.endVertex);
        runners[runnerId].travel(path, true);
        std::cout << time << " - Runner " << runnerId << " - assigned new job " << jobRequest.startVertex << " > " << jobRequest.endVertex << std::endl;
    }
}

void Simulation::finishRunnerJob(unsigned runnerId) {
    if(!isJobAssignedToRunner(runnerId)) {
        std::ostringstream message;
        message << "Failed to finish job on runner '" << runnerId << "': There is no job assigned to this runner.";
        throw std::runtime_error(message.str());
    }

    const auto jobRequest = jobAssignments[runnerId];
    jobAssignments[runnerId] = std::nullopt;
    finishedJobRequests.push_back(*jobRequest);
    std::cout << time << " - Runner " << runnerId << " - finished job " << jobRequest->startVertex << " > " << jobRequest->endVertex << std::endl;
}

void Simulation::advance() {
    assignNewJobsToRunners();

    // Move runners, prevent colisions
    for(unsigned runnerId = 0; runnerId < runners.size(); ++runnerId) {
        runners[runnerId].advance();
        std::cout << time << " - Runner " << runnerId << " at vertex " << runners[runnerId].getLastVisitedVertex() << " position " << runners[runnerId].getPosition() << std::endl;
    }

    // Finish jobs
    for(unsigned runnerId = 0; runnerId < runners.size(); ++runnerId) {
        if(runners[runnerId].isInDestination() && isJobAssignedToRunner(runnerId)) {
            finishRunnerJob(runnerId);            
        }
    }

    // Adjust time
    ++time;
}

bool Simulation::isFinished() const {
    bool areAllRunnersFinished = true;
    for(unsigned runnerId = 0; runnerId < runners.size(); ++runnerId) {
        areAllRunnersFinished &= !isJobAssignedToRunner(runnerId);
        areAllRunnersFinished &= runners[runnerId].isInDestination();
    }

    return newJobRequests.empty() && areAllRunnersFinished;
}

unsigned Simulation::getTime() const {
    return time;
}

