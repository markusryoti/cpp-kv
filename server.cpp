#include "kv.h"
#include "thread_pool.h"

int main() {
  ThreadPool::Pool tp{5};
  KV::Store store{8080, tp};

  store.Listen();
}
