#pragma once

#include <optional>
#include <string>

namespace Command {

struct Cmd {
    std::string method;
    std::string key;
    std::optional<std::string> value;
};

Cmd parse_request(const std::string &request);

} // namespace Command
