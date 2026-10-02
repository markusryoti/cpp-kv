# C++ KV store

C++ kv store with raw TCP sockets and simple Redis style interface. Thread pool for handling connections.

## Build

```bash
cmake -S . -B build && cmake --build build
```

## Test

```
ctest --test-dir build --output-on-failure
```
