#pragma once

#include <boost/icl/interval_map.hpp>
#include <map>
#include <optional>
#include <utility>
#include <vector>

#include "graph.h"
#include "runner.h"

class Constraints
{
 public:
  Constraints() = delete;
  Constraints(const WeightedDiGraph &graph);

  bool isVertexFreeForRunner(
      const Vertex &vertex,
      RunnerId runnerId,
      unsigned startTime = std::numeric_limits<unsigned>::min(),
      unsigned endTime = std::numeric_limits<unsigned>::max()) const;

  // bool isVertexLockedForRunner(
  //     const Vertex &vertex,
  //     RunnerId runnerId,
  //     unsigned startTime = std::numeric_limits<unsigned>::min(),
  //     unsigned endTime = std::numeric_limits<unsigned>::max()) const;

  bool lockVertex(
      const Vertex &vertex,
      RunnerId runnerId,
      unsigned startTime = std::numeric_limits<unsigned>::min(),
      unsigned endTime = std::numeric_limits<unsigned>::max());

  void unlockVertex(
      const Vertex &vertex,
      RunnerId runnerId,
      unsigned startTime = std::numeric_limits<unsigned>::min(),
      unsigned endTime = std::numeric_limits<unsigned>::max());

  std::optional<RunnerId> getVertexLock(const Vertex &vertex, unsigned time) const;

  /// @brief Checks whether a runner can traverse the directed edge `from` -> `to` during the given
  /// time interval without swapping places with another runner travelling `to` -> `from` at the
  /// same time (a "swap"/edge collision: two runners crossing the same edge head-on).
  bool isEdgeFreeForRunner(
      const Vertex &from,
      const Vertex &to,
      RunnerId runnerId,
      unsigned startTime = std::numeric_limits<unsigned>::min(),
      unsigned endTime = std::numeric_limits<unsigned>::max()) const;

  /// @brief Reserves the directed edge `from` -> `to` for `runnerId` during the given time interval.
  /// Fails (returns false, reserves nothing) if another runner is locked to traverse the reverse
  /// edge `to` -> `from` during an overlapping interval.
  bool lockEdge(
      const Vertex &from,
      const Vertex &to,
      RunnerId runnerId,
      unsigned startTime = std::numeric_limits<unsigned>::min(),
      unsigned endTime = std::numeric_limits<unsigned>::max());

  void unlockEdge(
      const Vertex &from,
      const Vertex &to,
      RunnerId runnerId,
      unsigned startTime = std::numeric_limits<unsigned>::min(),
      unsigned endTime = std::numeric_limits<unsigned>::max());

 protected:
  typedef std::set<RunnerId> VertexLockIntervalType;
  typedef boost::icl::interval_map<unsigned, VertexLockIntervalType> VertexLocksType;
  std::vector<VertexLocksType> locks;

  typedef std::pair<Vertex, Vertex> DirectedEdge;
  std::map<DirectedEdge, VertexLocksType> edgeLocks;
};
