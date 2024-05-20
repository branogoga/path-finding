#pragma once

#include <filesystem>
#include <vector>

#include "graph.h"
#include "runner.h"

void write_graph_to_dot_file(
    const std::filesystem::path &filename, WeightedDiGraph &graph, const std::vector<Runner> &runners = {});
