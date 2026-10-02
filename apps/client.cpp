#include <iostream>
#include <netinet/in.h>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

constexpr int kPort = 8080;

int get_socket() {
  int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

  sockaddr_in serverAddress;
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons(kPort);
  serverAddress.sin_addr.s_addr = INADDR_ANY;

  int s_val = connect(clientSocket, (struct sockaddr *)&serverAddress,
                      sizeof(serverAddress));

  return clientSocket;
}

void send_command(std::string_view msg) {
  int sock = get_socket();
  if (sock == -1) {
    throw std::runtime_error("coudln't obtain socket");
  }

  send(sock, msg.data(), msg.size(), 0);

  char buffer[1024] = {0};
  recv(sock, buffer, sizeof(buffer), 0);

  std::cout << "Message from server: " << buffer << std::endl;

  close(sock);
}

int main() {
  std::cout << "Client started" << std::endl;

  std::string cmd;

  while (std::getline(std::cin, cmd)) {
    if (cmd.starts_with('q')) {
      return 0;
    }

    send_command(cmd);
  }

  return 0;
}
