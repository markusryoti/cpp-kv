#include "kv_core/map.h"
#include <gtest/gtest.h>
#include <optional>

TEST(KvMap, PutAndGetValue) {
  KvMap::Map map;

  map.put("X", "7");
  auto val = map.get("X");

  ASSERT_TRUE(val.has_value());
  ASSERT_EQ(*val, "7");
}

TEST(KvMap, NonExistent) {
  KvMap::Map map;

  auto val = map.get("X");

  EXPECT_FALSE(val.has_value());
}
