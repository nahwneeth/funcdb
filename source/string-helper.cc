#include "string-helper.h"

namespace funcdb {

std::string ReadInput(std::string const& prompt) {
  std::cout << prompt;
  auto input = std::string();
  std::getline(std::cin, input);
  Trim(input);
  return input;
}

void Trim(std::string& self) {
  auto start = 0LL;
  auto end = (long long)(self.size()) - 1;

  while (start <= end && std::isspace(self[start])) ++start;
  while (end >= start && std::isspace(self[end])) --end;

  if (start >= end)
    self = std::string();
  else
    self = self.substr(start, end - start + 1);
}

bool StartsWith(std::string const& self, std::string const& start) {
  if (self.size() < start.size()) return false;

  if (self.substr(0, start.size()) == start)
    return true;
  else
    return false;
}

std::vector<std::string> Split(std::string const& self) {
  auto iss = std::istringstream(self);
  auto res = std::vector<std::string>();

  std::string part;
  while (iss >> part) {
    res.push_back(part);
  }

  return res;
}

}  // namespace funcdb
