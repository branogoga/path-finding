#include "strings.h"

#include <gtest/gtest.h>

#include <filesystem>

TEST(Strings, split)
{
  std::vector<std::string> result = split("ab cd ef\tgh", {' ', '\t'});
  std::vector<std::string> expected = {"ab", "cd", "ef", "gh"};
  ASSERT_EQ(result, expected);
}
