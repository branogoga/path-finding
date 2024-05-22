#include "sequence.h"

#include <gtest/gtest.h>

TEST(LinearSequence, default_unsigned)
{
  LinearSequence<unsigned> sequence;
  EXPECT_EQ(sequence(), 0u);
  EXPECT_EQ(sequence(), 1u);
  EXPECT_EQ(sequence(), 2u);
  EXPECT_EQ(sequence(), 3u);
  EXPECT_EQ(sequence(), 4u);
  EXPECT_EQ(sequence(), 5u);
  EXPECT_EQ(sequence(), 6u);
  EXPECT_EQ(sequence(), 7u);
}

TEST(LinearSequence, default_float)
{
  LinearSequence<float> sequence;
  EXPECT_EQ(sequence(), 0.0f);
  EXPECT_EQ(sequence(), 1.0f);
  EXPECT_EQ(sequence(), 2.0f);
  EXPECT_EQ(sequence(), 3.0f);
  EXPECT_EQ(sequence(), 4.0f);
  EXPECT_EQ(sequence(), 5.0f);
  EXPECT_EQ(sequence(), 6.0f);
  EXPECT_EQ(sequence(), 7.0f);
}

TEST(LinearSequence, start_unsigned)
{
  LinearSequence<unsigned> sequence(3);
  EXPECT_EQ(sequence(), 3u);
  EXPECT_EQ(sequence(), 4u);
  EXPECT_EQ(sequence(), 5u);
  EXPECT_EQ(sequence(), 6u);
  EXPECT_EQ(sequence(), 7u);
}

TEST(LinearSequence, start_negative_int)
{
  LinearSequence<int> sequence(-3);
  EXPECT_EQ(sequence(), -3);
  EXPECT_EQ(sequence(), -2);
  EXPECT_EQ(sequence(), -1);
  EXPECT_EQ(sequence(), 0);
  EXPECT_EQ(sequence(), 1);
  EXPECT_EQ(sequence(), 2);
  EXPECT_EQ(sequence(), 3);
}

TEST(LinearSequence, step_unsigned)
{
  LinearSequence<unsigned> sequence(1, 3);
  EXPECT_EQ(sequence(), 1u);
  EXPECT_EQ(sequence(), 4u);
  EXPECT_EQ(sequence(), 7u);
  EXPECT_EQ(sequence(), 10u);
}

TEST(LinearSequence, step_negative_integer)
{
  LinearSequence<int> sequence(5, -2);
  EXPECT_EQ(sequence(), +5);
  EXPECT_EQ(sequence(), +3);
  EXPECT_EQ(sequence(), +1);
  EXPECT_EQ(sequence(), -1);
}

TEST(LinearSequence, step_float)
{
  const float precision = 1E-03f;
  LinearSequence<float> sequence(0.3f, -0.1f);
  EXPECT_NEAR(sequence(), +0.3f, precision);
  EXPECT_NEAR(sequence(), +0.2f, precision);
  EXPECT_NEAR(sequence(), +0.1f, precision);
  EXPECT_NEAR(sequence(), 0.0f, precision);
  EXPECT_NEAR(sequence(), -0.1f, precision);
  EXPECT_NEAR(sequence(), -0.2f, precision);
  EXPECT_NEAR(sequence(), -0.3f, precision);
}

TEST(AlternateSequence, default_int)
{
  AlternateSequence<int> sequence;
  EXPECT_EQ(sequence(), +1);
  EXPECT_EQ(sequence(), -1);
  EXPECT_EQ(sequence(), +1);
  EXPECT_EQ(sequence(), -1);
  EXPECT_EQ(sequence(), +1);
  EXPECT_EQ(sequence(), -1);
  EXPECT_EQ(sequence(), +1);
  EXPECT_EQ(sequence(), -1);
}

TEST(AlternateSequence, negative_start)
{
  AlternateSequence<int> sequence(-2);
  EXPECT_EQ(sequence(), -2);
  EXPECT_EQ(sequence(), +2);
  EXPECT_EQ(sequence(), -2);
  EXPECT_EQ(sequence(), +2);
  EXPECT_EQ(sequence(), -2);
  EXPECT_EQ(sequence(), +2);
  EXPECT_EQ(sequence(), -2);
}
