#include "kv_core/kv.h"
#include "kv_core/thread_pool.h"
#include "spdlog/spdlog.h"

int main() {
    spdlog::set_level(spdlog::level::debug);

    ThreadPool::Pool tp{5};
    KV::Store store{8080, tp};

    store.Listen();
}
