#include "geometry.h"

#include <gtest/gtest.h>

TEST(Point2D, equality)
{
  Point2D point({0.3f, 1.7f});
  const float smallDelta = 10.0f * std::numeric_limits<float>::epsilon();
  const float tinyDelta = 0.1f * std::numeric_limits<float>::epsilon();
  EXPECT_TRUE(point == Point2D({0.3f, 1.7f}));
  EXPECT_FALSE(point == Point2D({0.3f + smallDelta, 1.7f}));
  EXPECT_FALSE(point == Point2D({0.3f - smallDelta, 1.7f}));
  EXPECT_FALSE(point == Point2D({0.3f, 1.7f + smallDelta}));
  EXPECT_FALSE(point == Point2D({0.3f, 1.7f - smallDelta}));
  EXPECT_TRUE(point == Point2D({0.3f + tinyDelta, 1.7f}));
  EXPECT_TRUE(point == Point2D({0.3f - tinyDelta, 1.7f}));
  EXPECT_TRUE(point == Point2D({0.3f, 1.7f + tinyDelta}));
  EXPECT_TRUE(point == Point2D({0.3f, 1.7f - tinyDelta}));
}

TEST(Point2D, isNear)
{
  Point2D point({0.3f, 1.7f});
  const float precision = 1E-03f;
  const float smallDelta = 1E-06f;
  const float bigDelta = 1E-02f;
  EXPECT_TRUE(isNear(point, Point2D({0.3f, 1.7f}), precision));
  EXPECT_TRUE(isNear(point, Point2D({0.3f + smallDelta, 1.7f}), precision));
  EXPECT_TRUE(isNear(point, Point2D({0.3f, 1.7f + smallDelta}), precision));
  EXPECT_FALSE(isNear(point, Point2D({0.3f + bigDelta, 1.7f}), precision));
  EXPECT_FALSE(isNear(point, Point2D({0.3f, 1.7f + bigDelta}), precision));
}

TEST(Point2D, print_to_stream)
{
  Point2D point({0.3f, 1.7f});
  std::ostringstream out;
  out << point;
  EXPECT_EQ(out.str(), "[ 0.3,1.7 ]");
}
