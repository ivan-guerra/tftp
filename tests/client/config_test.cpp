#include "client/config.h"

#include <gtest/gtest.h>

#include <string>

constexpr std::string kDefaultMode = "ascii";
constexpr std::string kDefaultPortRange = "2000:65535";
constexpr std::string kDefaultHost = "localhost";

TEST(ConfigTest, CreateReturnsValidConfig) {
  auto conf = tftp::client::Config::Create(kDefaultMode, kDefaultPortRange,
                                           true, kDefaultHost);
  ASSERT_TRUE(conf);
  ASSERT_TRUE(*conf);
}

TEST(ConfigTest, CreateReturnsUnknownModeWhenGivenInvalidMode) {
  auto conf = tftp::client::Config::Create("foo", kDefaultPortRange, true,
                                           kDefaultHost);
  ASSERT_FALSE(conf);
  ASSERT_EQ(conf.error(), tftp::client::Config::ErrorCode::kUnknownMode);
}

TEST(ConfigTest,
     CreateReturnsMissingRangeSeperatorWhenRangeDoesNotHaveSeperator) {
  auto conf =
      tftp::client::Config::Create(kDefaultMode, "2000", true, kDefaultHost);
  ASSERT_FALSE(conf);
  ASSERT_EQ(conf.error(),
            tftp::client::Config::ErrorCode::kMissingRangeSeperator);
}

TEST(ConfigTest, CreateReturnsPortNumIsNotUint16OnNegativePort) {
  auto conf = tftp::client::Config::Create(kDefaultMode, "-1:65535", true,
                                           kDefaultHost);
  ASSERT_FALSE(conf);
  ASSERT_EQ(conf.error(), tftp::client::Config::ErrorCode::kPortNumIsNotUint16);
}

TEST(ConfigTest, CreateReturnsPortNumOutOfRangeOnPortGreaterThanUint16Max) {
  auto conf = tftp::client::Config::Create(kDefaultMode, "2000:65536", true,
                                           kDefaultHost);
  ASSERT_FALSE(conf);
  ASSERT_EQ(conf.error(), tftp::client::Config::ErrorCode::kPortNumOutOfRange);
}

TEST(ConfigTest, CreateReturnsPortNumOutOfOrderOnStartPortGreaterThanEndPort) {
  auto conf = tftp::client::Config::Create(kDefaultMode, "65535:2000", true,
                                           kDefaultHost);
  ASSERT_FALSE(conf);
  ASSERT_EQ(conf.error(), tftp::client::Config::ErrorCode::kPortNumOutOfOrder);
}

TEST(ConfigTest, SetModeReturnsNulloptWhenGivenValidMode) {
  auto conf = tftp::client::Config::Create(kDefaultMode, kDefaultPortRange,
                                           true, kDefaultHost);
  ASSERT_TRUE(conf);
  ASSERT_TRUE(*conf);

  const char* kValidModes[] = {"ascii", "netascii", "octet", "binary",
                               "ASCII", "NETASCII", "OCTET", "BINARY"};
  std::optional<tftp::client::Config::ErrorCode> result = std::nullopt;
  for (const auto& mode : kValidModes) {
    result = (*conf)->SetMode(mode);
    ASSERT_FALSE(result);
  }
}

TEST(ConfigTest, SetModeReturnsUnknownModeWhenGivenInvalidMode) {
  auto conf = tftp::client::Config::Create(kDefaultMode, kDefaultPortRange,
                                           true, kDefaultHost);
  ASSERT_TRUE(conf);
  ASSERT_TRUE(*conf);

  auto result = (*conf)->SetMode("foo");
  ASSERT_TRUE(result);
  ASSERT_EQ(*result, tftp::client::Config::ErrorCode::kUnknownMode);
}

TEST(ConfigTest,
     SetPortRangeReturnsMissingRangeSeperatorWhenRangeDoesNotHaveSeperator) {
  auto conf = tftp::client::Config::Create(kDefaultMode, kDefaultPortRange,
                                           true, kDefaultHost);
  ASSERT_TRUE(conf);
  ASSERT_TRUE(*conf);

  auto result = (*conf)->SetPortRange("2000");
  ASSERT_TRUE(result);
  ASSERT_EQ(*result, tftp::client::Config::ErrorCode::kMissingRangeSeperator);
}

TEST(ConfigTest, SetPortRangeReturnsPortNumIsNotUint16OnNegativePort) {
  auto conf = tftp::client::Config::Create(kDefaultMode, kDefaultPortRange,
                                           true, kDefaultHost);
  ASSERT_TRUE(conf);
  ASSERT_TRUE(*conf);

  auto result = (*conf)->SetPortRange("-1:65535");
  ASSERT_TRUE(result);
  ASSERT_EQ(*result, tftp::client::Config::ErrorCode::kPortNumIsNotUint16);
}

TEST(ConfigTest,
     SetPortRangeReturnsPortNumOutOfRangeOnPortGreaterThanUint16Max) {
  auto conf = tftp::client::Config::Create(kDefaultMode, kDefaultPortRange,
                                           true, kDefaultHost);
  ASSERT_TRUE(conf);
  ASSERT_TRUE(*conf);

  auto result = (*conf)->SetPortRange("2000:65536");
  ASSERT_TRUE(result);
  ASSERT_EQ(*result, tftp::client::Config::ErrorCode::kPortNumOutOfRange);
}

TEST(ConfigTest,
     SetPortRangeReturnsPortNumOutOfOrderWhenStartPortGreaterThanEndPort) {
  auto conf = tftp::client::Config::Create(kDefaultMode, kDefaultPortRange,
                                           true, kDefaultHost);
  ASSERT_TRUE(conf);
  ASSERT_TRUE(*conf);

  auto result = (*conf)->SetPortRange("65535:2000");
  ASSERT_TRUE(result);
  ASSERT_EQ(*result, tftp::client::Config::ErrorCode::kPortNumOutOfOrder);
}
