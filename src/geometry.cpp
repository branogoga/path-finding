#include "geometry.h"

bool isNear(const Point2D& p1, const Point2D& p2, const float precision)
{
  return std::abs(p2.x - p1.x) < precision && std::abs(p2.y - p1.y) < precision;
}

bool operator==(const Point2D& p1, const Point2D& p2)
{
  return p1.x == p2.x && p1.y == p2.y;
}

std::ostream& operator<<(std::ostream& stream, const Point2D& point)
{
  stream << "[ " << point.x << "," << point.y << " ]";
  return stream;
}
