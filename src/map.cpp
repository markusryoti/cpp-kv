#include "kv_core/map.h"
#include <optional>
#include <utility>

namespace KvMap {

void Map::put(const std::string &key, std::string value) {
  {
    std::unique_lock<std::shared_mutex> lock(m_);
    store_[key] = std::move(value);
  }
}

std::optional<std::string> Map::get(const std::string &key) {
  {
    std::shared_lock<std::shared_mutex> guard(m_);
    auto it = store_.find(key);
    if (it != store_.end()) {
      return it->second;
    }
    return std::nullopt;
  }
}

} // namespace KvMap
