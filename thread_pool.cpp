#include "thread_pool.h"
#include <cstddef>
#include <mutex>

namespace ThreadPool {

Pool::Pool(std::size_t num_workers) {
  for (int i = 0; i < num_workers; i++) {
    workers.emplace_back([this]() { worker_loop(); });
  }
};
Pool::~Pool() { stopping = false; };

void Pool::enqueue_request(std::function<void()> task) {
  if (stopping) {
    return;
  }

  {
    std::lock_guard lock(mutex);
    tasks.emplace(task);
  }

  condition.notify_one();
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

    task();
  }
}

} // namespace ThreadPool
