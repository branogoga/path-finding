#include "strings.h"

std::vector<std::string> split(const std::string& input, const std::set<char>&& delimiters)
{
  std::vector<std::string> views;

  auto begin_of_word = input.begin();
  auto end_of_word = begin_of_word;

  while (end_of_word != input.end())
  {
    // check if input character can be found in delimiter
    while (end_of_word != input.end() && (delimiters.find(*end_of_word) == delimiters.end())) ++end_of_word;

    // double delimiter will result in empty view being added (should be fine)
    views.emplace_back(std::string(begin_of_word, end_of_word));

    // next word starts one after delimiter
    if (end_of_word != input.end()) begin_of_word = ++end_of_word;
  }

  return views;
}
