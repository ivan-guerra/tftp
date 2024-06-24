#ifndef CONFIG_H_
#define CONFIG_H_

#include <expected>
#include <memory>
#include <string_view>

#include "common/parse.h"
#include "common/types.h"

namespace tftp {
namespace client {

class Config;
using ConfigPtr = std::shared_ptr<Config>;

class Config {
 public:
  static std::expected<ConfigPtr, ParseStatus> Create(
      std::string_view mode, std::string_view port_range, bool literal_mode,
      const Hostname& hostname, std::string_view timeout,
      std::string_view rexmt_timeout);

  const Mode& GetMode() const { return mode_; }
  PortRange GetPortRange() const { return ports_; }
  bool GetLiteralMode() const { return literal_mode_; }
  const Hostname& GetHostname() const { return hostname_; }
  Seconds GetTimeout() const { return timeout_; }
  Seconds GetRexmtTimeout() const { return rexmt_timeout_; }

  std::optional<ParseStatus> SetMode(std::string_view mode);
  std::optional<ParseStatus> SetPortRange(std::string_view port_range);
  void SetLiteralMode(bool literal_mode) { literal_mode_ = literal_mode; }
  void SetHostname(const Hostname& hostname) { hostname_ = hostname; }
  std::optional<ParseStatus> SetTimeout(std::string_view timeout);
  void SetTimeout(Seconds timeout) { timeout_ = timeout; }
  std::optional<ParseStatus> SetRexmtTimeout(std::string_view rexmt_timeout);

 private:
  Config() = delete;
  Config(const tftp::Mode& mode, const struct PortRange& port_range,
         bool literal_mode, const Hostname& hostname, Seconds timeout,
         Seconds rexmt_timeout)
      : mode_(mode),
        ports_(port_range),
        literal_mode_(literal_mode),
        hostname_(hostname),
        timeout_(timeout),
        rexmt_timeout_(rexmt_timeout) {}

  tftp::Mode mode_;
  struct PortRange ports_;
  bool literal_mode_ = false;
  Hostname hostname_;
  Seconds timeout_;
  Seconds rexmt_timeout_;
};

}  // namespace client
}  // namespace tftp

#endif
