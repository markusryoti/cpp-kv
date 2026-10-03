#include "kv_core/kv.h"
#include "kv_core/command.h"
#include "spdlog/spdlog.h"
#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <netinet/in.h>
#include <optional>
#include <stdexcept>
#include <string>
#include <unistd.h>

namespace KV {

constexpr size_t kReadChunk = 1024;
constexpr size_t kMaxRequest = 4096;
constexpr time_t kReadTimeoutSec = 5;

Store::Store(uint16_t port, ThreadPool::Pool &pool) : pool_(pool) {
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

        spdlog::debug("Socket accepted, num_socket={}", client_socket);

        auto f = [this, client_socket]() {
            this->handle_request(client_socket);
        };

        pool_.enqueue_request(f);
    }
}

std::optional<std::string> read_request(int client_socket) {
    std::string request;
    char buffer[kReadChunk];

    while (true) {
        ssize_t n = recv(client_socket, buffer, sizeof(buffer), 0);
        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }
            spdlog::error("recv failed, num_socket={} errno={}", client_socket,
                          errno);
            return std::nullopt;
        }
        if (n == 0) {
            spdlog::error("Client closed before end of request, num_socket={}",
                          client_socket);
            return std::nullopt;
        }

        request.append(buffer, static_cast<size_t>(n));

        auto pos = request.find('\n');
        if (pos != std::string::npos && pos <= kMaxRequest) {
            request.resize(pos);
            return request;
        }
        if (request.size() > kMaxRequest) {
            spdlog::error("Request too large, num_socket={} size={}",
                          client_socket, request.size());
            return std::nullopt;
        }
    }
}

void Store::handle_request(int client_socket) {
    spdlog::debug("Handling socket, num_socket={}", client_socket);

    timeval tv{.tv_sec = kReadTimeoutSec, .tv_usec = 0};
    setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    auto request = read_request(client_socket);
    if (!request) {
        const std::string err = "ERR request too large or malformed";
        send(client_socket, err.data(), err.size(), 0);
        close(client_socket);
        return;
    }

    spdlog::debug("Read client data for socket, num_socket={} size={}",
                  client_socket, request->size());

    try {
        auto cmd = Command::parse_request(*request);

        if (cmd.method == "SET") {
            if (cmd.value.has_value()) {
                std::string value = *cmd.value;
                map_.put(cmd.key, value);
                send(client_socket, value.data(), value.length(), 0);
            }
        } else if (cmd.method == "GET") {
            auto value = map_.get(cmd.key).value_or("");
            send(client_socket, value.data(), value.length(), 0);
        } else {
            spdlog::error("Unexpected method, num_socket={} method={}",
                          client_socket, cmd.method);
            send(client_socket, "", strlen(""), 0);
        }
    } catch (const std::runtime_error &e) {
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

} // namespace KV
