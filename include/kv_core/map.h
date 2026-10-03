#pragma once

#include <map>
#include <optional>
#include <shared_mutex>
#include <string>

namespace KvMap {

class Map {
  public:
    void put(const std::string &key, std::string value);
    std::optional<std::string> get(const std::string &key);

  private:
    std::map<std::string, std::string> store_;
    std::shared_mutex m_;
};

} // namespace KvMap
