#include "kv_core/command.h"
#include <cctype>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace Command {

Cmd parse_request(const std::string &request) {
    std::stringstream ss(request);
    std::vector<std::string> tokens;
    std::string s;

    while (ss >> s) {
        std::transform(s.begin(), s.end(), s.begin(), ::toupper);
        tokens.push_back(s);
    }

    if (tokens.empty()) {
        throw std::runtime_error("empty_command");
    }

    if (tokens.size() == 2) {
        return Cmd{
            .method = tokens.at(0), .key = tokens.at(1), .value = std::nullopt};
    } else if (tokens.size() == 3) {
        return Cmd{
            .method = tokens.at(0), .key = tokens.at(1), .value = tokens.at(2)};
    } else {
        std::stringstream ss;
        ss << "invalid_command=" << tokens.at(0)
           << " num_parts=" << tokens.size();
        throw std::runtime_error(ss.str());
    }
}

} // namespace Command
