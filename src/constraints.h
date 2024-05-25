#pragma once

#include <optional>
#include <vector>

#include "graph.h"
#include "runner.h"

class Constraints
{
 public:
  Constraints() = delete;
  Constraints(const WeightedDiGraph &graph);

  bool isVertexFreeForRunner(const Vertex &vertex, RunnerId runnerId) const;
  bool lockVertex(const Vertex &vertex, RunnerId runnerId);
  void unlockVertex(const Vertex &vertex);
  const std::optional<RunnerId> &getVertexLock(const Vertex &vertex) const;

 private:
  std::vector<std::optional<RunnerId>> vertexLocks;
  // TODO: edge locks
};
