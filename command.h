#include <iostream>
#include <string>
#include <vector>

namespace Command {

struct Cmd {
  std::string method;
  std::string key;
  std::optional<std::string> value;
};

Cmd parse_request(char *buffer);

} // namespace Command
