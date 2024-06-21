#ifndef CONFIG_H_
#define CONFIG_H_

#include <array>
#include <cstdint>
#include <expected>
#include <memory>
#include <optional>
#include <string_view>

#include "common/types.h"

namespace tftp {
namespace client {

class Config;
using ConfigPtr = std::shared_ptr<Config>;

class Config {
 public:
  enum ErrorCode {
    kUnknownMode = 0,
    kMissingRangeSeperator,
    kPortNumOutOfRange,
    kPortNumIsNotUint16,
    kPortNumOutOfOrder,
    kNumCodes,
  };

  struct PortRange {
    uint16_t start = 0;
    uint16_t end = 0;
  };

  static constexpr std::array<const char*, ErrorCode::kNumCodes> kRetCodeToStr{
      "unknown mode -> expected 'ascii'/'netascii' or 'binary'/'octet'",
      "missing range seperator -> expected ':' between port numbers",
      "port number is out of range -> expected [0, 65535]",
      "specified port is not numeric -> expected uint16",
      "port range is out of order -> expected FIRST:LAST where FIRST <= LAST",
  };

  static std::expected<ConfigPtr, ErrorCode> Create(std::string_view mode,
                                                    std::string_view port_range,
                                                    bool literal_mode,
                                                    const HostName& host);

  const Mode& GetMode() const { return mode_; }
  PortRange GetPortRange() const { return ports_; }
  bool GetLiteralMode() const { return literal_mode_; }
  const HostName& GetHost() const { return host_; }

  std::optional<Config::ErrorCode> SetMode(std::string_view mode);
  std::optional<Config::ErrorCode> SetPortRange(std::string_view port_range);
  void SetLiteralMode(bool literal_mode) { literal_mode_ = literal_mode; }
  void SetHost(const HostName& host) { host_ = host; }

 private:
  Config() = delete;
  Config(const tftp::Mode& mode, const struct PortRange& port_range,
         bool literal_mode, const HostName& host)
      : mode_(mode),
        ports_(port_range),
        literal_mode_(literal_mode),
        host_(host) {}

  tftp::Mode mode_;
  struct PortRange ports_;
  bool literal_mode_ = false;
  HostName host_;
};

}  // namespace client
}  // namespace tftp

#endif
