include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)
FetchContent_MakeAvailable(googletest)

add_executable(kv_tests 
  command_test.cpp
  thread_pool_test.cpp
  map_test.cpp
)

target_link_libraries(kv_tests PRIVATE kv_core GTest::gtest_main)
include(GoogleTest)

gtest_discover_tests(kv_tests)

