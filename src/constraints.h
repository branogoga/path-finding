#pragma once

#include <boost/icl/interval_map.hpp>
#include <optional>
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

 protected:
  typedef std::set<RunnerId> VertexLockIntervalType;
  typedef boost::icl::interval_map<unsigned, VertexLockIntervalType> VertexLocksType;
  std::vector<VertexLocksType> locks;
};
