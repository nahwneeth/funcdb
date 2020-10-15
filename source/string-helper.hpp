#if !defined(FUNCDB_STRING_HELPER_HPP)
#define FUNCDB_STRING_HELPER_HPP

#include <cctype>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

namespace funcdb
{

    std::string 
    read_input(std::string const& prompt = std::string());

    void 
    trim(std::string& self);

    bool
    starts_with(std::string const& self, std::string const& start);

    std::vector<std::string>
    split(std::string const& self);

} // namespace funcdb


#endif // FUNCDB_STRING_HELPER_HPP

