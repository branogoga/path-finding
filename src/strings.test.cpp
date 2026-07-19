#include "strings.h"

#include <gtest/gtest.h>

#include <filesystem>
#include <sstream>

TEST(Strings, split)
{
  std::vector<std::string> result = split("ab cd ef\tgh", {' ', '\t'});
  std::vector<std::string> expected = {"ab", "cd", "ef", "gh"};
  ASSERT_EQ(result, expected);
}

TEST(Strings, getLine_strips_trailing_carriage_return)
{
  std::istringstream stream("type octile\r\nheight 2\r\n");
  std::string line;
  getLine(stream, line);
  EXPECT_EQ(line, "type octile");
  getLine(stream, line);
  EXPECT_EQ(line, "height 2");
}

TEST(Strings, getLine_leaves_unix_line_endings_unchanged)
{
  std::istringstream stream("type octile\nheight 2\n");
  std::string line;
  getLine(stream, line);
  EXPECT_EQ(line, "type octile");
  getLine(stream, line);
  EXPECT_EQ(line, "height 2");
}
