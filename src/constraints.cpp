#include "constraints.h"

Constraints::Constraints(const WeightedDiGraph& graph) : vertexLocks(graph.m_vertices.size(), std::nullopt)
{
}

bool Constraints::isVertexFreeForRunner(const Vertex& vertex, RunnerId runnerId) const
{
  return vertexLocks[vertex] == std::nullopt || vertexLocks[vertex] == runnerId;
}

bool Constraints::lockVertex(const Vertex& vertex, RunnerId runnerId)
{
  if (!isVertexFreeForRunner(vertex, runnerId))
  {
    return false;
  }
  vertexLocks[vertex] = runnerId;
  return true;
}

void Constraints::unlockVertex(const Vertex& vertex)
{
  vertexLocks[vertex] = std::nullopt;
}

const std::optional<RunnerId>& Constraints::getVertexLock(const Vertex& vertex) const
{
  return vertexLocks[vertex];
}
