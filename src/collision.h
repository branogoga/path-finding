#pragma once

#include <vector>

#include "graph.h"


typedef std::tuple<unsigned, unsigned, std::vector<Vertex>> Intersection;
std::vector<Intersection> getIntersections(const std::vector<Path>& paths);
