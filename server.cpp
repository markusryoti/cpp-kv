#include "kv.h"
#include <iostream>
#include <optional>
#include <string>

#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

void print_res(std::string key, std::optional<std::string> val) {
  std::cout << "key: " << key << " value=" << val.value_or("none") << std::endl;
}

int main() {
  KV::Store store{8080};

  store.Listen();
}
