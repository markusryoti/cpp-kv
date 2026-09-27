#include "kv.h"
#include "command.h"
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <optional>
#include <ostream>
#include <stdexcept>
#include <string>
#include <unistd.h>

namespace KV {

Store::Store(int port) {
  server_socket_ = socket(AF_INET, SOCK_STREAM, 0);

  sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(port);
  server_address.sin_addr.s_addr = INADDR_ANY;

  int opt = 1;

  setsockopt(server_socket_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  int bind_val = bind(server_socket_, (struct sockaddr *)&server_address,
                      sizeof(server_address));
}

void Store::Listen() {
  listen(server_socket_, 5);

  std::cout << "listening" << std::endl;

  while (server_socket_ != -1) {
    int client_socket = accept(server_socket_, nullptr, nullptr);

    std::cout << "request accepted" << std::endl;

    char buffer[1024] = {0};
    recv(client_socket, buffer, sizeof(buffer), 0);

    std::cout << "received request: " << sizeof(buffer) << std::endl;

    try {
      auto cmd = Command::parse_request(buffer);

      if (cmd.method == "SET") {
        if (cmd.value.has_value()) {
          std::string value = cmd.value.value();
          store_[cmd.key] = value;
          std::cout << "setting: " << cmd.key << "=" << cmd.value.value_or("")
                    << std::endl;
          send(client_socket, value.c_str(), value.length(), 0);
        }
      } else if (cmd.method == "GET") {
        auto value = this->get(cmd.key).value_or("");
        std::cout << "getting: " << cmd.key << "=" << value << std::endl;
        send(client_socket, value.c_str(), value.length(), 0);
      } else {
        throw std::runtime_error("unexpected error");
      }
    } catch (std::runtime_error e) {
      send(client_socket, e.what(), strlen(e.what()), 0);
    }

    close(client_socket);
  }
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
