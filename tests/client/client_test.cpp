#include <gtest/gtest.h>

#include <string>

#include "client/config.h"

constexpr std::string kDefaultMode = "ascii";
constexpr std::string kDefaultPortRange = "2000:65535";
constexpr std::string kDefaultHost = "localhost";

TEST(ClientTest, CreateReturnsValidConfig) {
  auto conf = tftp::client::Config::Create(kDefaultMode, kDefaultPortRange,
                                           true, kDefaultHost);
  ASSERT_TRUE(conf);
}

TEST(ClientTest, CreateReturnsUnknownModeWhenGivenInvalidMode) {
  auto conf = tftp::client::Config::Create("foo", kDefaultPortRange, true,
                                           kDefaultHost);
  ASSERT_FALSE(conf);
  ASSERT_EQ(conf.error(), tftp::client::Config::ErrorCode::kUnknownMode);
}

TEST(ClientTest,
     CreateReturnsMissingRangeSeperatorWhenRangeDoesNotHaveSeperator) {
  auto conf =
      tftp::client::Config::Create(kDefaultMode, "2000", true, kDefaultHost);
  ASSERT_FALSE(conf);
  ASSERT_EQ(conf.error(),
            tftp::client::Config::ErrorCode::kMissingRangeSeperator);
}

TEST(ClientTest, CreateReturnsPortNumIsNotUint16OnNegativePort) {
  auto conf = tftp::client::Config::Create(kDefaultMode, "-1:65535", true,
                                           kDefaultHost);
  ASSERT_FALSE(conf);
  ASSERT_EQ(conf.error(), tftp::client::Config::ErrorCode::kPortNumIsNotUint16);
}

TEST(ClientTest, CreateReturnsPortNumOutOfRangeOnPortGreaterThanUint16Max) {
  auto conf = tftp::client::Config::Create(kDefaultMode, "2000:65536", true,
                                           kDefaultHost);
  ASSERT_FALSE(conf);
  ASSERT_EQ(conf.error(), tftp::client::Config::ErrorCode::kPortNumOutOfRange);
}

TEST(ClientTest, CreateReturnsPortNumOutOfOrderOnStartPortGreaterThanEndPort) {
  auto conf = tftp::client::Config::Create(kDefaultMode, "65535:2000", true,
                                           kDefaultHost);
  ASSERT_FALSE(conf);
  ASSERT_EQ(conf.error(), tftp::client::Config::ErrorCode::kPortNumOutOfOrder);
}

TEST(ClientTest, SetModeReturnsNulloptWhenGivenValidMode) {
  auto conf = tftp::client::Config::Create(kDefaultMode, kDefaultPortRange,
                                           true, kDefaultHost);
  ASSERT_TRUE(conf);

  const char* kValidModes[] = {"ascii", "netascii", "octet", "binary",
                               "ASCII", "NETASCII", "OCTET", "BINARY"};
  std::optional<tftp::client::Config::ErrorCode> result = std::nullopt;
  for (const auto& mode : kValidModes) {
    result = conf->SetMode(mode);
    ASSERT_FALSE(result);
  }
}

TEST(ClientTest, SetModeReturnsUnknownModeWhenGivenInvalidMode) {
  auto conf = tftp::client::Config::Create(kDefaultMode, kDefaultPortRange,
                                           true, kDefaultHost);
  ASSERT_TRUE(conf);

  auto result = conf->SetMode("foo");
  ASSERT_TRUE(result);
  ASSERT_EQ(*result, tftp::client::Config::ErrorCode::kUnknownMode);
}

TEST(ClientTest,
     SetPortRangeReturnsMissingRangeSeperatorWhenRangeDoesNotHaveSeperator) {
  auto conf = tftp::client::Config::Create(kDefaultMode, kDefaultPortRange,
                                           true, kDefaultHost);
  ASSERT_TRUE(conf);

  auto result = conf->SetPortRange("2000");
  ASSERT_TRUE(result);
  ASSERT_EQ(*result, tftp::client::Config::ErrorCode::kMissingRangeSeperator);
}

TEST(ClientTest, SetPortRangeReturnsPortNumIsNotUint16OnNegativePort) {
  auto conf = tftp::client::Config::Create(kDefaultMode, kDefaultPortRange,
                                           true, kDefaultHost);
  ASSERT_TRUE(conf);

  auto result = conf->SetPortRange("-1:65535");
  ASSERT_TRUE(result);
  ASSERT_EQ(*result, tftp::client::Config::ErrorCode::kPortNumIsNotUint16);
}

TEST(ClientTest,
     SetPortRangeReturnsPortNumOutOfRangeOnPortGreaterThanUint16Max) {
  auto conf = tftp::client::Config::Create(kDefaultMode, kDefaultPortRange,
                                           true, kDefaultHost);
  ASSERT_TRUE(conf);

  auto result = conf->SetPortRange("2000:65536");
  ASSERT_TRUE(result);
  ASSERT_EQ(*result, tftp::client::Config::ErrorCode::kPortNumOutOfRange);
}

TEST(ClientTest,
     SetPortRangeReturnsPortNumOutOfOrderWhenStartPortGreaterThanEndPort) {
  auto conf = tftp::client::Config::Create(kDefaultMode, kDefaultPortRange,
                                           true, kDefaultHost);
  ASSERT_TRUE(conf);

  auto result = conf->SetPortRange("65535:2000");
  ASSERT_TRUE(result);
  ASSERT_EQ(*result, tftp::client::Config::ErrorCode::kPortNumOutOfOrder);
}
