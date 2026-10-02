#include "kv_core/kv.h"
#include "kv_core/command.h"
#include "spdlog/spdlog.h"
#include <cstring>
#include <netinet/in.h>
#include <optional>
#include <shared_mutex>
#include <stdexcept>
#include <string>
#include <unistd.h>

namespace KV {

Store::Store(int port, ThreadPool::Pool &pool) : pool_(pool) {
  server_socket_ = socket(AF_INET, SOCK_STREAM, 0);

  sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(port);
  server_address.sin_addr.s_addr = INADDR_ANY;

  int opt = 1;

  setsockopt(server_socket_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  int _ = bind(server_socket_, (struct sockaddr *)&server_address,
               sizeof(server_address));
}

void Store::Listen() {
  spdlog::info("Starting server");

  listen(server_socket_, 5);

  spdlog::info("Server socket listening");

  while (server_socket_ != -1) {
    int client_socket = accept(server_socket_, nullptr, nullptr);

    spdlog::info("Socket accepted, num_socket={}", client_socket);

    auto f = [this, client_socket]() { this->handle_request(client_socket); };

    pool_.enqueue_request(f);
  }
}

void Store::handle_request(int client_socket) {
  spdlog::debug("Handling socket, num_socket={}", client_socket);

  char buffer[1024] = {0};

  int _ = recv(client_socket, buffer, sizeof(buffer), 0);

  spdlog::debug("Read client data for socket, num_socket={} size={}",
                client_socket, strlen(buffer));

  try {
    auto cmd = Command::parse_request(buffer);

    if (cmd.method == "SET") {
      if (cmd.value.has_value()) {
        std::string value = cmd.value.value();
        {
          std::unique_lock<std::shared_mutex> lock(m_);
          store_[cmd.key] = value;
        }
        send(client_socket, value.c_str(), value.length(), 0);
      }
    } else if (cmd.method == "GET") {
      std::string value;
      {
        std::shared_lock<std::shared_mutex> guard(m_);
        value = this->get(cmd.key).value_or("");
      }
      send(client_socket, value.data(), value.length(), 0);
    } else {
      spdlog::error("Unexpected method, num_socket={} method={}", client_socket,
                    cmd.method);
      send(client_socket, "", strlen(""), 0);
    }
  } catch (std::runtime_error e) {
    spdlog::error("Runtime error, num_socket={}, error={}", client_socket,
                  e.what());
    send(client_socket, e.what(), strlen(e.what()), 0);
  }

  close(client_socket);

  spdlog::debug("Socket handled, num_socket={}", client_socket);
}

void Store::Stop() {
  close(server_socket_);
  server_socket_ = -1;
}

void Store::put(std::string key, std::string value) {
  store_.insert({key, value});
}

std::optional<std::string> Store::get(std::string &key) {
  try {
    auto val = store_.at(key);
    return val;
  } catch (const std::out_of_range) {
    return std::nullopt;
  }
}

} // namespace KV
