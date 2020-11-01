#if !defined(FUNCDB_STRING_HELPER_H)
#define FUNCDB_STRING_HELPER_H

#include <cctype>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace funcdb {

std::string ReadInput(std::string const& prompt = std::string());

void Trim(std::string& self);

bool StartsWith(std::string const& self, std::string const& start);

std::vector<std::string> Split(std::string const& self);

}  // namespace funcdb

#endif  // FUNCDB_STRING_HELPER_H
