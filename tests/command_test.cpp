#include "kv_core/command.h"
#include <gtest/gtest.h>
#include <stdexcept>

TEST(ParseRequest, GetWithoutValue) {
  std::string req{"GET X"};
  auto res = Command::parse_request(req);

  ASSERT_EQ(res.method, "GET");
  ASSERT_EQ(res.key, "X");
  EXPECT_FALSE(res.value.has_value());
}

TEST(ParseRequest, ThrowEmpty) {
  std::string req{""};
  EXPECT_THROW(Command::parse_request(req), std::runtime_error);
}
