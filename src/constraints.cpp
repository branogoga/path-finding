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

bool Constraints::isEdgeFreeForRunner(
    const Vertex& from, const Vertex& to, RunnerId runnerId, unsigned startTime, unsigned endTime) const
{
  // A runner travelling `from` -> `to` collides head-on with any other runner travelling the
  // reverse edge `to` -> `from` during an overlapping interval, so that's the only direction that
  // needs checking here: two runners travelling the same direction on the same edge can never
  // overlap in time in the first place, since they would first have to occupy the same `from`
  // vertex at the same time, which vertex locking already prevents.
  const auto reverseEdgeIterator = edgeLocks.find(DirectedEdge(to, from));
  if (reverseEdgeIterator == edgeLocks.end())
  {
    return true;
  }
  const auto& interval_map = reverseEdgeIterator->second;
  const auto range = interval_map.equal_range(boost::icl::interval<unsigned>::right_open(startTime, endTime));
  for (auto it = range.first; it != range.second; ++it)
  {
    auto edgeLockedToRunners = it->second;
    if (!std::all_of(
            edgeLockedToRunners.begin(),
            edgeLockedToRunners.end(),
            [&runnerId](RunnerId id) { return runnerId == id; }))
    {
      return false;
    }
  }
  // Invariant: There are no intervals in that range, or all intervals belong to given `runnerId`.
  return true;
}

bool Constraints::lockEdge(
    const Vertex& from, const Vertex& to, RunnerId runnerId, unsigned startTime, unsigned endTime)
{
  if (!isEdgeFreeForRunner(from, to, runnerId, startTime, endTime))
  {
    return false;
  }
  edgeLocks[DirectedEdge(from, to)].add(std::make_pair(
      boost::icl::interval<unsigned>::right_open(startTime, endTime), VertexLockIntervalType({runnerId})));

  return true;
}

void Constraints::unlockEdge(
    const Vertex& from, const Vertex& to, RunnerId runnerId, unsigned startTime, unsigned endTime)
{
  const auto edgeIterator = edgeLocks.find(DirectedEdge(from, to));
  if (edgeIterator != edgeLocks.end())
  {
    edgeIterator->second -= std::make_pair(
        boost::icl::interval<unsigned>::right_open(startTime, endTime), VertexLockIntervalType({runnerId}));
  }
}
