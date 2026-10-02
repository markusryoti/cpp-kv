cmake_minimum_required(VERSION 3.25)

project(
    cpp-kv
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(kv_core STATIC
    src/kv.cpp
    src/command.cpp
    src/thread_pool.cpp
)

find_package(Threads REQUIRED)
find_package(spdlog REQUIRED)
target_link_libraries(kv_core PUBLIC Threads::Threads spdlog::spdlog)

target_include_directories(kv_core
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_compile_options(kv_core PRIVATE -Wall -Wextra -Wpedantic)

add_executable(server apps/server.cpp)
target_link_libraries(server PRIVATE kv_core)

add_executable(client apps/client.cpp)
target_link_libraries(client PRIVATE kv_core)


enable_testing()
add_subdirectory(tests)
