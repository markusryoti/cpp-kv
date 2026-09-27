#include "command.h"
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

constexpr int kPort = 8080;

int get_socket() {
  int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

  // specifying address
  sockaddr_in serverAddress;
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons(kPort);
  serverAddress.sin_addr.s_addr = INADDR_ANY;

  // sending connection request
  int s_val = connect(clientSocket, (struct sockaddr *)&serverAddress,
                      sizeof(serverAddress));

  return clientSocket;
}

int main() {
  // write data
  int sock1 = get_socket();
  const char *message = "SET X 7";
  send(sock1, message, strlen(message), 0);
  close(sock1);

  // get
  int sock2 = get_socket();
  const char *req = "GET X";
  send(sock2, req, strlen(req), 0);

  char buffer[1024] = {0};
  recv(sock2, buffer, sizeof(buffer), 0);

  std::cout << "Message from server: " << buffer << std::endl;

  close(sock2);

  return 0;
}
