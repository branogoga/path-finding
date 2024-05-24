#pragma once

#include <optional>
#include <vector>

#include "graph.h"

class Constraints
{
 public:
  Constraints() = delete;
  Constraints(const WeightedDiGraph &graph);

  bool isVertexFreeForRunner(const Vertex &vertex, unsigned runnerId) const;
  bool lockVertex(const Vertex &vertex, unsigned runnerId);
  void unlockVertex(const Vertex &vertex);
  const std::optional<unsigned> &getVertexLock(const Vertex &vertex) const;

 private:
  std::vector<std::optional<unsigned>> vertexLocks;
  // TODO: edge locks
};
