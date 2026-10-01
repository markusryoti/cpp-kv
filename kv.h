#include "thread_pool.h"
#include <map>
#include <optional>
#include <shared_mutex>

namespace KV {

class Store {
public:
  Store(int port, ThreadPool::Pool &pool);

  void Listen();
  void Stop();

  void put(std::string key, std::string value);
  std::optional<std::string> get(std::string &key);

private:
  int server_socket_ = -1;

  ThreadPool::Pool &pool_;
  std::map<std::string, std::string> store_;
  std::shared_mutex m_;

  void handle_request(int client_socket);
};

} // namespace KV
