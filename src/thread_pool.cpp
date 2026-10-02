#include "kv_core/thread_pool.h"
#include "spdlog/spdlog.h"
#include <cstddef>
#include <mutex>

namespace ThreadPool {

Pool::Pool(std::size_t num_workers) {
  for (size_t i = 0; i < num_workers; i++) {
    workers.emplace_back([this]() { worker_loop(); });
  }
};

Pool::~Pool() {
  {
    std::lock_guard lock(mutex);
    stopping = true;
  }

  condition.notify_all();

  for (auto &w : workers) {
    w.join();
  }
};

void Pool::enqueue_request(std::function<void()> task) {
  {
    std::lock_guard lock(mutex);
    if (stopping) {
      return;
    }
    tasks.emplace(task);
  }

  condition.notify_one();

  spdlog::debug("Request enqueued for thread pool");
}

void Pool::worker_loop() {
  while (true) {
    std::function<void()> task;

    {
      std::unique_lock<std::mutex> lock(mutex);

      condition.wait(lock, [this] { return stopping || !tasks.empty(); });

      if (stopping && tasks.empty()) {
        return;
      }

      task = std::move(tasks.front());
      tasks.pop();
    }

    spdlog::debug("Popped request from thread pool");

    task();
  }
}

} // namespace ThreadPool
