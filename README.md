# C++ KV store

C++ kv store with raw TCP sockets and simple Redis style interface. Thread pool for handling connections.

## Build

```bash
cmake -S . -B build
cmake -S . -B build-tsan -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="-fsanitize=thread -g"
cmake --build build && ctest --test-dir build --output-on-failure
cmake --build build-tsan && ctest --test-dir build-tsan --output-on-failure
```

## Test

```bash
ctest --test-dir build --output-on-failure
ctest --test-dir build-tsan --output-on-failure
```
