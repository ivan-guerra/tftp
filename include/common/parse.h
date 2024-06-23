#ifndef PARSE_H_
#define PARSE_H_

#include <array>
#include <expected>
#include <string_view>

#include "common/types.h"

namespace tftp {

enum ParseStatus : int {
  kSuccessfulParse = 0,
  kUnknownCmd,
  kInvalidNumArgs,
  kPortNumOutOfRange,
  kPortNumOutOfOrder,
  kPortRangeMissingSeperator,
  kUnknownMode,
  kTimeoutOutOfRange,
  kParseStatusCnt,
};

constexpr std::array<const char*, ParseStatus::kParseStatusCnt>
    kParseStatusToStr = {
        "success",
        "unknown command",
        "invalid number of arguments",
        "port number is out of range [0, 65535]",
        "port number range is not an increasing range",
        "port range is missing seperator ':'",
        "unknown transfer mode",
        "timeout is out of range [0, 65535]",
};

std::expected<tftp::Mode, ParseStatus> ParseMode(std::string_view val);
std::expected<uint16_t, ParseStatus> ParsePort(std::string_view val);
std::expected<PortRange, ParseStatus> ParsePortRange(std::string_view val);
std::expected<Seconds, ParseStatus> ParseTimeValue(std::string_view val);

}  // namespace tftp

#endif
