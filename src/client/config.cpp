#include "client/config.h"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <expected>
#include <limits>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

#include "common/types.h"

namespace tftp {
namespace client {

static std::expected<tftp::Mode, Config::ErrorCode> ParseMode(
    std::string_view mode) {
  std::string mode_lower(mode.size(), 0);
  std::transform(mode.cbegin(), mode.cend(), mode_lower.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  if (mode_lower == "ascii" || mode_lower == "netascii") {
    return tftp::SendMode::kNetAscii;
  } else if (mode_lower == "binary" || mode_lower == "octet") {
    return tftp::SendMode::kOctet;
  } else {
    return std::unexpected(Config::ErrorCode::kUnknownMode);
  }
}

static std::expected<Config::PortRange, Config::ErrorCode> ParsePortRange(
    std::string_view range) {
  auto IsPositiveNum = [](std::string_view val) {
    return std::all_of(val.cbegin(), val.cend(),
                       [](char c) { return std::isdigit(c); });
  };

  std::size_t seperator = range.find(':');
  if (seperator == std::string::npos) {
    return std::unexpected(Config::ErrorCode::kMissingRangeSeperator);
  }

  std::string_view start_str = range.substr(0, seperator);
  std::string_view end_str = range.substr(seperator + 1);
  if (!IsPositiveNum(start_str) || !IsPositiveNum(end_str)) {
    return std::unexpected(Config::ErrorCode::kPortNumIsNotUint16);
  }

  uint64_t start = std::stoull(start_str.data());
  uint64_t end = std::stoull(end_str.data());
  uint16_t max = std::numeric_limits<uint16_t>::max();
  if ((start > max) || (end > max)) {
    return std::unexpected(Config::ErrorCode::kPortNumOutOfRange);
  }

  if (start > end) {
    return std::unexpected(Config::ErrorCode::kPortNumOutOfOrder);
  }

  Config::PortRange port_range = {.start = static_cast<uint16_t>(start),
                                  .end = static_cast<uint16_t>(end)};
  return port_range;
}

std::expected<ConfigPtr, Config::ErrorCode> Config::Create(
    std::string_view mode, std::string_view port_range, bool literal_mode,
    const HostName& host) {
  auto parsed_mode = ParseMode(mode);
  if (!parsed_mode) {
    return std::unexpected(parsed_mode.error());
  }

  auto parsed_range = ParsePortRange(port_range);
  if (!parsed_range) {
    return std::unexpected(parsed_range.error());
  }

  /* Have to use a raw new here since std::make_shared<> cannot access the
   * private Config constructor. */
  return std::shared_ptr<Config>(
      new Config(*parsed_mode, *parsed_range, literal_mode, host));
}

std::optional<Config::ErrorCode> Config::SetMode(std::string_view mode) {
  auto parsed_mode = ParseMode(mode);
  if (!parsed_mode) {
    return parsed_mode.error();
  }

  mode_ = *parsed_mode;

  return std::nullopt;
}

std::optional<Config::ErrorCode> Config::SetPortRange(
    std::string_view port_range) {
  auto parsed_range = ParsePortRange(port_range);
  if (!parsed_range) {
    return parsed_range.error();
  }

  ports_ = *parsed_range;

  return std::nullopt;
}

}  // namespace client
}  // namespace tftp
