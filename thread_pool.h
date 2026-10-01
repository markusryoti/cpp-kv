#pragma once

#include <functional>
#include <queue>
#include <thread>

namespace ThreadPool {

class Pool {
public:
  Pool(std::size_t num_workers);
  ~Pool();

  void enqueue_request(std::function<void()> task);

private:
  void worker_loop();

  std::vector<std::thread> workers;
  std::queue<std::function<void()>> tasks;

  std::mutex mutex;
  std::condition_variable condition;
  bool stopping = false;
};

} // namespace ThreadPool
