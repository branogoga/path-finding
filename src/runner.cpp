#include "runner.h"

unsigned lastRunnerId = 0;

Runner::Runner(WeightedDiGraph graph, Vertex initialPosition)
    : id(++lastRunnerId)
    , graph(graph)
    , lastVisitedVertex(initialPosition)
    , destinationVertex(initialPosition)
    , indexOfLastVisitedPathVertex(0)
{
  position = graph.m_vertices[initialPosition].m_property.position;
}

unsigned Runner::getId() const
{
  return id;
}

const Path& Runner::getPath() const
{
  return path;
}

const Point2D& Runner::getPosition() const
{
  return position;
}

Vertex Runner::getLastVisitedVertex() const
{
  return lastVisitedVertex;
}

Vertex Runner::getNextVertex() const
{
  if (!path.empty() && indexOfLastVisitedPathVertex + 1 <= path.size() - 1)
  {
    return path[indexOfLastVisitedPathVertex + 1];
  } else
  {
    lastVisitedVertex;
  }
}

Vertex Runner::getDestinationVertex() const
{
  return destinationVertex;
}

bool Runner::isTraveling() const
{
  return lastVisitedVertex != destinationVertex;
}

bool Runner::isInDestination() const
{
  return lastVisitedVertex == destinationVertex;
}

void Runner::travel(const Path& trajectory, bool allowTeleport)
{
  path = trajectory;
  indexOfLastVisitedPathVertex = 0;
  if (!path.empty())
  {
    destinationVertex = path[path.size() - 1];
    if (path[0] != lastVisitedVertex)
    {
      if (allowTeleport)
      {
        lastVisitedVertex = path[0];
      } else
      {
        std::ostringstream message;
        message << "Runner '" << getId() << "' rejects the path, because it does not start at the current position '"
                << lastVisitedVertex << "', " << position << ", but starts at '" << path[0] << "', "
                << graph.m_vertices[path[0]].m_property.position << ", full path: " << path;
        throw std::runtime_error(message.str());
      }
    }
  }
}

void Runner::advance()
{
  const size_t size = path.size();
  if (path.size() >= 2 && indexOfLastVisitedPathVertex < path.size() - 1)
  {
    ++indexOfLastVisitedPathVertex;
    lastVisitedVertex = path[indexOfLastVisitedPathVertex];
    position = graph.m_vertices[lastVisitedVertex].m_property.position;
  }
}
