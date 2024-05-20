#include "collision.h"

std::vector<Intersection> getIntersections(const std::vector<Path>& paths)
{
  std::vector<Intersection> intersections;
  for (unsigned i = 0; i < paths.size(); ++i)
  {
    for (unsigned j = 0; j < paths.size(); ++j)
    {
      if (i < j)
      {
        const auto shared_vertices = intersection(paths[i], paths[j]);
        if (!shared_vertices.empty())
        {
          intersections.emplace_back(i, j, shared_vertices);
        }
      }
    }
  }
  return intersections;
}
