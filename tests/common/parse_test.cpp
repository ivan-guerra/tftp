
#include "common/parse.h"

#include <gtest/gtest.h>

#include <cstdint>
#include <string>
#include <unordered_map>

#include "common/types.h"

TEST(ParseTest, ParseModeReturnsValidModeWhenGiveValidModeStrs) {
  std::unordered_map<const char*, tftp::Mode> kValidModes = {
      {"ascii", tftp::SendMode::kNetAscii},
      {"netascii", tftp::SendMode::kNetAscii},
      {"octet", tftp::SendMode::kOctet},
      {"binary", tftp::SendMode::kOctet},
      {"ASCII", tftp::SendMode::kNetAscii},
      {"NETASCII", tftp::SendMode::kNetAscii},
      {"OCTET", tftp::SendMode::kOctet},
      {"BINARY", tftp::SendMode::kOctet},
  };

  for (const auto& kv : kValidModes) {
    auto parsed_mode = tftp::ParseMode(kv.first);
    ASSERT_TRUE(parsed_mode);
    ASSERT_EQ(*parsed_mode, kv.second);
  }
}

TEST(ParseTest, ParseModeReturnsUnknownModeWhenGivenInvalidMode) {
  ASSERT_FALSE(tftp::ParseMode("foo"));
}

TEST(ParseTest, ParsePortReturnsValidPortWhenGivenValidPortStr) {
  uint16_t port = 42;
  auto parsed_port = tftp::ParsePort(std::to_string(port));

  ASSERT_TRUE(parsed_port);
  ASSERT_EQ(port, *parsed_port);
}

TEST(ParseTest, ParsePortReturnsPortNumOutOfRangeOnNonNumericInput) {
  auto parsed_port = tftp::ParsePort("foo");

  ASSERT_FALSE(parsed_port);
  ASSERT_EQ(tftp::ParseStatus::kPortNumOutOfRange, parsed_port.error());
}

TEST(ParseTest, ParsePortReturnsPortNumOutOfRangeOnNegativeInput) {
  auto parsed_port = tftp::ParsePort("-42");

  ASSERT_FALSE(parsed_port);
  ASSERT_EQ(tftp::ParseStatus::kPortNumOutOfRange, parsed_port.error());
}

TEST(ParseTest, ParsePortReturnsPortNumOutOfRangeOnInputThatExceedsPortMax) {
  auto parsed_port = tftp::ParsePort("65536");

  ASSERT_FALSE(parsed_port);
  ASSERT_EQ(tftp::ParseStatus::kPortNumOutOfRange, parsed_port.error());
}

TEST(ParseTest, ParseTimeValueReturnsValidTimeWhenGivenValidTimeStr) {
  tftp::Seconds timeout = 60;
  auto parsed_time = tftp::ParseTimeValue(std::to_string(timeout));

  ASSERT_TRUE(parsed_time);
  ASSERT_EQ(timeout, *parsed_time);
}

TEST(ParseTest, ParsePortRangeReturnsValidRangeWhenGiveValidPortRangeInput) {
  uint16_t start = 5555;
  uint16_t end = 6666;
  auto parsed_range =
      tftp::ParsePortRange(std::to_string(start) + ":" + std::to_string(end));

  ASSERT_TRUE(parsed_range);
  ASSERT_EQ(start, parsed_range->start);
  ASSERT_EQ(end, parsed_range->end);
}

TEST(ParseTest, ParsePortRangeReturnsPortNumOutOfRangeOnNegativePort) {
  auto parsed_range = tftp::ParsePortRange("-5555:6666");

  ASSERT_FALSE(parsed_range);
  ASSERT_EQ(tftp::ParseStatus::kPortNumOutOfRange, parsed_range.error());
}

TEST(ParseTest,
     ParsePortRangeReturnsPortNumOutOfRangeOnPortGreaterThanPortMax) {
  auto parsed_range = tftp::ParsePortRange("5555:65536");

  ASSERT_FALSE(parsed_range);
  ASSERT_EQ(tftp::ParseStatus::kPortNumOutOfRange, parsed_range.error());
}

TEST(ParseTest,
     ParsePortRangeReturnsPortNumOutOfOrderOnStartPortGreaterThanEndPort) {
  auto parsed_range = tftp::ParsePortRange("6666:5555");

  ASSERT_FALSE(parsed_range);
  ASSERT_EQ(tftp::ParseStatus::kPortNumOutOfOrder, parsed_range.error());
}

TEST(ParseTest, ParseTimeReturnsTimeoutOutOfRangeWhenGivenNonNumericInput) {
  auto parsed_time = tftp::ParseTimeValue("foo");

  ASSERT_FALSE(parsed_time);
  ASSERT_EQ(tftp::ParseStatus::kTimeoutOutOfRange, *parsed_time);
}

TEST(ParseTest, ParseTimeReturnsTimeoutOutOfRangeWhenGivenNegative) {
  auto parsed_time = tftp::ParseTimeValue("-60");

  ASSERT_FALSE(parsed_time);
  ASSERT_EQ(tftp::ParseStatus::kTimeoutOutOfRange, *parsed_time);
}

TEST(ParseTest, ParseTimeReturnsTimeoutOutOfRangeOnInputThatExceedsTimeoutMax) {
  auto parsed_time = tftp::ParseTimeValue("65536");

  ASSERT_FALSE(parsed_time);
  ASSERT_EQ(tftp::ParseStatus::kTimeoutOutOfRange, *parsed_time);
}

TEST(ParseTest,
     ParsePortRangeReturnsMissingRangeSeperatorWhenRangeDoesNotHaveSeperator) {
  auto parsed_range = tftp::ParsePortRange("5555");

  ASSERT_FALSE(parsed_range);
  ASSERT_EQ(tftp::ParseStatus::kPortRangeMissingSeperator,
            parsed_range.error());
}
