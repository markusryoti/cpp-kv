#include "kv_core/kv.h"
#include "kv_core/thread_pool.h"

int main() {
  ThreadPool::Pool tp{5};
  KV::Store store{8080, tp};

  store.Listen();
}
