#pragma once

#include <set>
#include <string>
#include <vector>

std::vector<std::string> split(const std::string& input, const std::set<char>&& delimiters);
