#include "kv_core/thread_pool.h"
#include <atomic>
#include <cstddef>
#include <gtest/gtest.h>
#include <latch>

TEST(ThreadPool, RunsAllTasks) {
    auto pool = ThreadPool::Pool(4);

    std::atomic<int> a;
    std::latch done{100};

    for (size_t i = 0; i < 100; i++) {
        pool.enqueue_request([&a, &done]() {
            a++;
            done.count_down();
        });
    }

    done.wait();

    ASSERT_EQ(a, 100);
}
