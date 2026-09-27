#include <map>
#include <optional>

namespace KV {

class Store {
public:
  Store(int port);

  void Listen();
  void Stop();

  void put(std::string key, std::string value);
  std::optional<std::string> get(std::string &key);

private:
  int server_socket_ = -1;
  std::map<std::string, std::string> store_;
};

} // namespace KV
