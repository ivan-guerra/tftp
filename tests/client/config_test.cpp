#include "client/config.h"

#include <gtest/gtest.h>

#include <string>

#include "common/parse.h"

constexpr std::string kDefaultMode = "ascii";
constexpr std::string kDefaultPortRange = "2000:65535";
constexpr std::string kDefaultHostname = "localhost";
constexpr std::string kDefaultTimeout = "60";
constexpr std::string kDefaultRexmtTimeout = "10";

TEST(ConfigTest, CreateReturnsValidConfig) {
  auto conf = tftp::client::Config::Create(
      kDefaultMode, kDefaultPortRange, true, kDefaultHostname, kDefaultTimeout,
      kDefaultRexmtTimeout);
  ASSERT_TRUE(conf);
  ASSERT_TRUE(*conf);
}

TEST(ConfigTest, CreateReturnsUnknownModeWhenGivenInvalidMode) {
  auto conf = tftp::client::Config::Create("foo", kDefaultPortRange, true,
                                           kDefaultHostname, kDefaultTimeout,
                                           kDefaultRexmtTimeout);
  ASSERT_FALSE(conf);
  ASSERT_EQ(conf.error(), tftp::ParseStatus::kUnknownMode);
}

TEST(ConfigTest,
     CreateReturnsMissingRangeSeperatorWhenRangeDoesNotHaveSeperator) {
  auto conf =
      tftp::client::Config::Create(kDefaultMode, "2000", true, kDefaultHostname,
                                   kDefaultTimeout, kDefaultRexmtTimeout);
  ASSERT_FALSE(conf);
  ASSERT_EQ(conf.error(), tftp::ParseStatus::kPortRangeMissingSeperator);
}

TEST(ConfigTest, CreateReturnsPortNumOutOfRangeOnNegativePort) {
  auto conf = tftp::client::Config::Create(kDefaultMode, "-1:65535", true,
                                           kDefaultHostname, kDefaultTimeout,
                                           kDefaultRexmtTimeout);
  ASSERT_FALSE(conf);
  ASSERT_EQ(conf.error(), tftp::ParseStatus::kPortNumOutOfRange);
}

TEST(ConfigTest, CreateReturnsPortNumOutOfRangeOnPortGreaterThanPortMax) {
  auto conf = tftp::client::Config::Create(kDefaultMode, "2000:65536", true,
                                           kDefaultHostname, kDefaultTimeout,
                                           kDefaultRexmtTimeout);
  ASSERT_FALSE(conf);
  ASSERT_EQ(conf.error(), tftp::ParseStatus::kPortNumOutOfRange);
}

TEST(ConfigTest, CreateReturnsPortNumOutOfOrderOnStartPortGreaterThanEndPort) {
  auto conf = tftp::client::Config::Create(kDefaultMode, "65535:2000", true,
                                           kDefaultHostname, kDefaultTimeout,
                                           kDefaultRexmtTimeout);
  ASSERT_FALSE(conf);
  ASSERT_EQ(conf.error(), tftp::ParseStatus::kPortNumOutOfOrder);
}

TEST(ConfigTest, CreateReturnsTimeoutOutOfRangeWhenTimeoutIsNotNumeric) {
  auto conf = tftp::client::Config::Create(kDefaultMode, kDefaultPortRange,
                                           true, kDefaultHostname, "foo",
                                           kDefaultRexmtTimeout);
  ASSERT_FALSE(conf);
  ASSERT_EQ(conf.error(), tftp::ParseStatus::kTimeoutOutOfRange);
}

TEST(ConfigTest, CreateReturnsTimeoutOutOfRangeWhenTimeoutIsNegative) {
  auto conf = tftp::client::Config::Create(kDefaultMode, kDefaultPortRange,
                                           true, kDefaultHostname, "-60",
                                           kDefaultRexmtTimeout);
  ASSERT_FALSE(conf);
  ASSERT_EQ(conf.error(), tftp::ParseStatus::kTimeoutOutOfRange);
}

TEST(ConfigTest,
     CreateReturnsTimeoutOutOfRangeWhenTimeoutIsGreaterThanSecondsMax) {
  auto conf = tftp::client::Config::Create(kDefaultMode, kDefaultPortRange,
                                           true, kDefaultHostname, "65536",
                                           kDefaultRexmtTimeout);
  ASSERT_FALSE(conf);
  ASSERT_EQ(conf.error(), tftp::ParseStatus::kTimeoutOutOfRange);
}

TEST(ConfigTest, CreateReturnsTimeoutOutOfRangeWhenRexmtTimeoutIsNotNumeric) {
  auto conf =
      tftp::client::Config::Create(kDefaultMode, kDefaultPortRange, true,
                                   kDefaultHostname, kDefaultTimeout, "foo");
  ASSERT_FALSE(conf);
  ASSERT_EQ(conf.error(), tftp::ParseStatus::kTimeoutOutOfRange);
}

TEST(ConfigTest, CreateReturnsTimeoutOutOfRangeWhenRexmtTimeoutIsNegative) {
  auto conf =
      tftp::client::Config::Create(kDefaultMode, kDefaultPortRange, true,
                                   kDefaultHostname, kDefaultTimeout, "-60");
  ASSERT_FALSE(conf);
  ASSERT_EQ(conf.error(), tftp::ParseStatus::kTimeoutOutOfRange);
}

TEST(ConfigTest,
     CreateReturnsTimeoutOutOfRangeWhenRexmtTimeoutIsGreaterThanSecondsMax) {
  auto conf =
      tftp::client::Config::Create(kDefaultMode, kDefaultPortRange, true,
                                   kDefaultHostname, kDefaultTimeout, "65536");
  ASSERT_FALSE(conf);
  ASSERT_EQ(conf.error(), tftp::ParseStatus::kTimeoutOutOfRange);
}

TEST(ConfigTest, SetModeReturnsNulloptWhenGivenValidMode) {
  auto conf = tftp::client::Config::Create(
      kDefaultMode, kDefaultPortRange, true, kDefaultHostname, kDefaultTimeout,
      kDefaultRexmtTimeout);
  ASSERT_TRUE(conf);
  ASSERT_TRUE(*conf);

  const char* kValidModes[] = {"ascii", "netascii", "octet", "binary",
                               "ASCII", "NETASCII", "OCTET", "BINARY"};
  std::optional<tftp::ParseStatus> result = std::nullopt;
  for (const auto& mode : kValidModes) {
    result = (*conf)->SetMode(mode);
    ASSERT_FALSE(result);
  }
}

TEST(ConfigTest, SetModeReturnsUnknownModeWhenGivenInvalidMode) {
  auto conf = tftp::client::Config::Create(
      kDefaultMode, kDefaultPortRange, true, kDefaultHostname, kDefaultTimeout,
      kDefaultRexmtTimeout);
  ASSERT_TRUE(conf);
  ASSERT_TRUE(*conf);

  auto result = (*conf)->SetMode("foo");
  ASSERT_TRUE(result);
  ASSERT_EQ(*result, tftp::ParseStatus::kUnknownMode);
}

TEST(ConfigTest, SetPortRangeReturnsNulloptWhenGivenValidPortRange) {
  auto conf = tftp::client::Config::Create(
      kDefaultMode, kDefaultPortRange, true, kDefaultHostname, kDefaultTimeout,
      kDefaultRexmtTimeout);
  ASSERT_FALSE((*conf)->SetPortRange("5555:6666"));
}

TEST(ConfigTest,
     SetPortRangeReturnsMissingRangeSeperatorWhenRangeDoesNotHaveSeperator) {
  auto conf = tftp::client::Config::Create(
      kDefaultMode, kDefaultPortRange, true, kDefaultHostname, kDefaultTimeout,
      kDefaultRexmtTimeout);
  ASSERT_TRUE(conf);
  ASSERT_TRUE(*conf);

  auto result = (*conf)->SetPortRange("2000");
  ASSERT_TRUE(result);
  ASSERT_EQ(*result, tftp::ParseStatus::kPortRangeMissingSeperator);
}

TEST(ConfigTest, SetPortRangeReturnsPortNumOutOfRangeOnNegativePort) {
  auto conf = tftp::client::Config::Create(
      kDefaultMode, kDefaultPortRange, true, kDefaultHostname, kDefaultTimeout,
      kDefaultRexmtTimeout);
  ASSERT_TRUE(conf);
  ASSERT_TRUE(*conf);

  auto result = (*conf)->SetPortRange("-1:65535");
  ASSERT_TRUE(result);
  ASSERT_EQ(*result, tftp::ParseStatus::kPortNumOutOfRange);
}

TEST(ConfigTest, SetPortRangeReturnsPortNumOutOfRangeOnPortGreaterThanPortMax) {
  auto conf = tftp::client::Config::Create(
      kDefaultMode, kDefaultPortRange, true, kDefaultHostname, kDefaultTimeout,
      kDefaultRexmtTimeout);
  ASSERT_TRUE(conf);
  ASSERT_TRUE(*conf);

  auto result = (*conf)->SetPortRange("2000:65536");
  ASSERT_TRUE(result);
  ASSERT_EQ(*result, tftp::ParseStatus::kPortNumOutOfRange);
}

TEST(ConfigTest,
     SetPortRangeReturnsPortNumOutOfOrderWhenStartPortGreaterThanEndPort) {
  auto conf = tftp::client::Config::Create(
      kDefaultMode, kDefaultPortRange, true, kDefaultHostname, kDefaultTimeout,
      kDefaultRexmtTimeout);
  ASSERT_TRUE(conf);
  ASSERT_TRUE(*conf);

  auto result = (*conf)->SetPortRange("65535:2000");
  ASSERT_TRUE(result);
  ASSERT_EQ(*result, tftp::ParseStatus::kPortNumOutOfOrder);
}

TEST(ConfigTest, SetTimeoutReturnsTimeoutOutOfRangeWhenTimeoutIsNotNumeric) {
  auto conf = tftp::client::Config::Create(
      kDefaultMode, kDefaultPortRange, true, kDefaultHostname, kDefaultTimeout,
      kDefaultRexmtTimeout);

  ASSERT_TRUE(conf);
  ASSERT_TRUE(*conf);
  ASSERT_EQ((*conf)->SetTimeout("foo"), tftp::ParseStatus::kTimeoutOutOfRange);
}

TEST(ConfigTest, SetTimeoutReturnsTimeoutOutOfRangeWhenTimeoutIsNegative) {
  auto conf = tftp::client::Config::Create(
      kDefaultMode, kDefaultPortRange, true, kDefaultHostname, kDefaultTimeout,
      kDefaultRexmtTimeout);

  ASSERT_TRUE(conf);
  ASSERT_TRUE(*conf);
  ASSERT_EQ((*conf)->SetTimeout("-60"), tftp::ParseStatus::kTimeoutOutOfRange);
}

TEST(ConfigTest,
     SetTimeoutReturnsTimeoutOutOfRangeWhenTimeoutIsGreaterThanSecondsMax) {
  auto conf = tftp::client::Config::Create(
      kDefaultMode, kDefaultPortRange, true, kDefaultHostname, kDefaultTimeout,
      kDefaultRexmtTimeout);

  ASSERT_TRUE(conf);
  ASSERT_TRUE(*conf);
  ASSERT_EQ((*conf)->SetTimeout("65536"),
            tftp::ParseStatus::kTimeoutOutOfRange);
}

TEST(ConfigTest,
     SetRexmtTimeoutReturnsTimeoutOutOfRangeWhenTimeoutIsNotNumeric) {
  auto conf = tftp::client::Config::Create(
      kDefaultMode, kDefaultPortRange, true, kDefaultHostname, kDefaultTimeout,
      kDefaultRexmtTimeout);

  ASSERT_TRUE(conf);
  ASSERT_TRUE(*conf);
  ASSERT_EQ((*conf)->SetRexmtTimeout("foo"),
            tftp::ParseStatus::kTimeoutOutOfRange);
}

TEST(ConfigTest, SetRexmtTimeoutReturnsTimeoutOutOfRangeWhenTimeoutIsNegative) {
  auto conf = tftp::client::Config::Create(
      kDefaultMode, kDefaultPortRange, true, kDefaultHostname, kDefaultTimeout,
      kDefaultRexmtTimeout);

  ASSERT_TRUE(conf);
  ASSERT_TRUE(*conf);
  ASSERT_EQ((*conf)->SetRexmtTimeout("-60"),
            tftp::ParseStatus::kTimeoutOutOfRange);
}

TEST(
    ConfigTest,
    SetRexmtTimeoutReturnsTimeoutOutOfRangeWhenTimeoutIsGreaterThanSecondsMax) {
  auto conf = tftp::client::Config::Create(
      kDefaultMode, kDefaultPortRange, true, kDefaultHostname, kDefaultTimeout,
      kDefaultRexmtTimeout);

  ASSERT_TRUE(conf);
  ASSERT_TRUE(*conf);
  ASSERT_EQ((*conf)->SetRexmtTimeout("65536"),
            tftp::ParseStatus::kTimeoutOutOfRange);
}
