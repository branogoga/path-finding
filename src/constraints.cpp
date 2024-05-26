#include "constraints.h"

#include <iostream>

Constraints::Constraints(const WeightedDiGraph& graph) : locks(graph.m_vertices.size(), VertexLocksType())
{
}

bool Constraints::isVertexFreeForRunner(
    const Vertex& vertex, RunnerId runnerId, unsigned startTime, unsigned endTime) const
{
  const auto& interval_map = locks[vertex];
  const auto range = interval_map.equal_range(boost::icl::interval<unsigned>::right_open(startTime, endTime));
  for (auto it = range.first; it != range.second; ++it)
  {
    auto vertexLockedToRunners = it->second;
    if (!std::all_of(
            vertexLockedToRunners.begin(),
            vertexLockedToRunners.end(),
            [&runnerId](RunnerId id) { return runnerId == id; }))
    {
      return false;
    }
  }
  // Invariant: There are no intervals in that range, or all intervals belong to given `runnerId`.
  return true;
}

bool Constraints::lockVertex(const Vertex& vertex, RunnerId runnerId, unsigned startTime, unsigned endTime)
{
  if (!isVertexFreeForRunner(vertex, runnerId, startTime, endTime))
  {
    return false;
  }
  locks[vertex].add(std::make_pair(
      boost::icl::interval<unsigned>::right_open(startTime, endTime), VertexLockIntervalType({runnerId})));

  return true;
}

void Constraints::unlockVertex(const Vertex& vertex, RunnerId runnerId, unsigned startTime, unsigned endTime)
{
  locks[vertex] -= std::make_pair(
      boost::icl::interval<unsigned>::right_open(startTime, endTime), VertexLockIntervalType({runnerId}));
}

std::optional<RunnerId> Constraints::getVertexLock(const Vertex& vertex, unsigned time) const
{
  const auto& interval_map = locks[vertex];
  auto interval_pair = interval_map.lower_bound(boost::icl::interval<unsigned>::closed(time, time));
  if (interval_pair != interval_map.end() && boost::icl::contains(interval_pair->first, time))
  {
    VertexLockIntervalType lockedTo = interval_pair->second;
    return lockedTo.begin().operator*();
  }
  else
  {
    return std::nullopt;
  }
}
