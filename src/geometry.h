#pragma once

#include <ostream>

class Point2D
{
 public:
  float x;
  float y;
};

bool operator==(const Point2D& p1, const Point2D& p2);
bool isNear(const Point2D& p1, const Point2D& p2, const float precision);
std::ostream& operator<<(std::ostream& stream, const Point2D& point);
