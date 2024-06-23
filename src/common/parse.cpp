#include "common/parse.h"

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <limits>
#include <string>
#include <string_view>

#include "common/types.h"

namespace tftp {

static bool IsPositiveNum(std::string_view val) {
  return std::all_of(val.cbegin(), val.cend(),
                     [](char c) { return std::isdigit(c); });
}

std::expected<tftp::Mode, ParseStatus> ParseMode(std::string_view val) {
  std::string mode_lower(val.size(), 0);
  std::transform(val.cbegin(), val.cend(), mode_lower.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  if (mode_lower == "ascii" || mode_lower == "netascii") {
    return tftp::SendMode::kNetAscii;
  } else if (mode_lower == "binary" || mode_lower == "octet") {
    return tftp::SendMode::kOctet;
  } else {
    return std::unexpected(ParseStatus::kUnknownMode);
  }
}

std::expected<uint16_t, ParseStatus> ParsePort(std::string_view val) {
  if (!IsPositiveNum(val)) {
    return std::unexpected(ParseStatus::kPortNumOutOfRange);
  }

  uint64_t port_tmp = std::stoull(val.data());
  if (port_tmp > std::numeric_limits<uint16_t>::max()) {
    return std::unexpected(ParseStatus::kPortNumOutOfRange);
  }

  return static_cast<uint16_t>(port_tmp);
}

std::expected<PortRange, ParseStatus> ParsePortRange(std::string_view val) {
  std::size_t seperator = val.find(':');
  if (seperator == std::string::npos) {
    return std::unexpected(ParseStatus::kPortRangeMissingSeperator);
  }

  auto start_port = ParsePort(val.substr(0, seperator));
  if (!start_port) {
    return std::unexpected(start_port.error());
  }
  auto end_port = ParsePort(val.substr(seperator + 1));
  if (!end_port) {
    return std::unexpected(end_port.error());
  }

  if (*start_port > *end_port) {
    return std::unexpected(ParseStatus::kPortNumOutOfOrder);
  }

  PortRange port_range{.start = *start_port, .end = *end_port};
  return port_range;
}

std::expected<Seconds, ParseStatus> ParseTimeValue(std::string_view num_str) {
  if (!IsPositiveNum(num_str)) {
    return std::unexpected(ParseStatus::kTimeoutOutOfRange);
  }

  uint64_t timeout_tmp = std::stoull(num_str.data());
  if (timeout_tmp > std::numeric_limits<uint16_t>::max()) {
    return std::unexpected(ParseStatus::kTimeoutOutOfRange);
  }

  return timeout_tmp;
}

}  // namespace tftp
