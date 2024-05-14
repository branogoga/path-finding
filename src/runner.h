#pragma once

#include "graph.h"

class Runner {
public:
    Runner() = delete;
    Runner(WeightedDiGraph graph, Vertex initialPosition);

    unsigned getId() const;
    const Path& getPath() const;
    const Point2D& getPosition() const;
    Vertex getLastVisitedVertex() const;
    Vertex getNextVertex() const;
    Vertex getDestinationVertex() const;

    bool isTraveling() const;
    bool isInDestination() const;

    void travel(const Path& path, bool allowTeleport = false);
    void advance();

    // TODO: notify observers when reached the destination
    // TODO: notify observers when the travel starts

private:
    unsigned id;
    WeightedDiGraph graph;
    Path path;
    unsigned indexOfLastVisitedPathVertex;
    Point2D position;
    Vertex lastVisitedVertex;
    Vertex destinationVertex;
};