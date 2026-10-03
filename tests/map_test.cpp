#include "kv_core/map.h"
#include <cstddef>
#include <gtest/gtest.h>
#include <optional>
#include <string>
#include <thread>
#include <vector>

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

TEST(KvMap, Overwrite) {
    KvMap::Map map;

    map.put("X", "7");
    map.put("X", "9");
    auto val = map.get("X");

    ASSERT_TRUE(val.has_value());
    ASSERT_EQ(*val, "9");
}

TEST(KvMap, Concurrency) {
    KvMap::Map map;

    size_t num_threads = 10;
    size_t keys_per_thread = 100;

    std::vector<std::thread> tasks;

    for (size_t i = 0; i < num_threads; i++) {
        auto f = [i, keys_per_thread, &map]() {
            for (size_t j = 0; j < keys_per_thread; j++) {
                auto k = std::to_string(i * keys_per_thread + j);
                auto key = "key-" + k;
                map.put(key, k);
            }
        };

        tasks.emplace_back(f);
    }

    for (auto &t : tasks) {
        t.join();
    }

    for (size_t i = 0; i < num_threads * keys_per_thread; i++) {
        auto k = std::to_string(i);
        auto val = map.get("key-" + k);

        ASSERT_EQ(val, k);
    }
}
