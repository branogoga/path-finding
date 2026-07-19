#pragma once

#include <istream>
#include <set>
#include <string>
#include <vector>

std::vector<std::string> split(const std::string& input, const std::set<char>&& delimiters);

/// @brief Reads a line the same way std::getline does, but also strips a trailing '\r' so that
/// files with Windows (CRLF) line endings parse the same way on platforms whose text streams
/// don't strip it themselves (e.g. Linux).
std::istream& getLine(std::istream& input, std::string& line);
