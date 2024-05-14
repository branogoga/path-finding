#include "simulation.h"

#include <iostream>

Simulation::Simulation(const std::vector<JobRequest>& jobRequests, const WeightedDiGraph& graph, unsigned numberOfRunners) 
: newJobRequests(jobRequests.rbegin(), jobRequests.rend())  // Revert the list, so that we can quickly pop first job from back
, jobAssignments(numberOfRunners, std::nullopt)
, graph(graph)
, vertexLocks(graph.m_vertices.size(), std::nullopt)
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
        unlockVertex(runners[runnerId].getLastVisitedVertex());
        runners[runnerId].travel(path, true);
        bool isLocked = lockVertex(runners[runnerId].getLastVisitedVertex(), runnerId);
        if(!isLocked) {
            std::ostringstream message;
            message << "Failed to assign new job to runner '" << runnerId << "': Vertex " << runners[runnerId].getLastVisitedVertex() << " is already locked to runner " << *vertexLocks[runners[runnerId].getLastVisitedVertex()];
            throw std::runtime_error(message.str());
        }
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

void Simulation::finishRunnerJobs() {
    for(unsigned runnerId = 0; runnerId < runners.size(); ++runnerId) {
        if(runners[runnerId].isInDestination() && isJobAssignedToRunner(runnerId)) {
            finishRunnerJob(runnerId);            
        }
    }
}

void Simulation::moveRunners() {
    for(unsigned runnerId = 0; runnerId < runners.size(); ++runnerId) {
        auto& runner = runners[runnerId];
        if(lockVertex(runner.getNextVertex(), runnerId)) {
            const auto previousVertex = runner.getLastVisitedVertex();
            runner.advance();
            if(previousVertex != runner.getLastVisitedVertex()) {
                unlockVertex(previousVertex);
            }
            std::cout << time << " - Runner " << runnerId << " moved to vertex " << runner.getLastVisitedVertex() << " position " << runner.getPosition() << std::endl;
        } else {
            std::cout << time << " - Runner " << runnerId << " stays at vertex " << runner.getLastVisitedVertex() << " position " << runner.getPosition() << ". Next vertex " << runner.getNextVertex() << " is locked to runner" << *vertexLocks[runner.getNextVertex()] << std::endl;
        }
    }
}

void Simulation::advance() {
    assignNewJobsToRunners();
    moveRunners();
    finishRunnerJobs();
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

bool Simulation::isVertexFreeForRunner(const Vertex& vertex, unsigned runnerId) const {
    return vertexLocks[vertex] == std::nullopt || vertexLocks[vertex] == runnerId;
}

bool Simulation::lockVertex(const Vertex& vertex, unsigned runnerId) {
    if(!isVertexFreeForRunner(vertex, runnerId)) {
        return false;
    }
    vertexLocks[vertex] = runnerId;
    return true;
}

void Simulation::unlockVertex(const Vertex& vertex) {
    vertexLocks[vertex] = std::nullopt;
}
