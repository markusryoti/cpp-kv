#pragma once

#include "kv_core/map.h"
#include "kv_core/thread_pool.h"
#include <cstdint>

namespace KV {

class Store {
  public:
    Store(uint16_t port, ThreadPool::Pool &pool);

    void Listen();
    void Stop();

  private:
    void handle_request(int client_socket);

    int server_socket_ = -1;
    ThreadPool::Pool &pool_;
    KvMap::Map map_;
};

} // namespace KV
