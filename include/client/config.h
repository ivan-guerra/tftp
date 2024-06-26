#ifndef CONFIG_H_
#define CONFIG_H_

#include "common/types.h"

namespace tftp {
namespace client {

struct Config {
  tftp::Mode mode = SendMode::kNetAscii;
  struct PortRange ports = {.start = 0, .end = 0};
  bool literal_mode = false;
  Hostname hostname = "localhost";
  Seconds timeout = 0;
  Seconds rexmt_timeout = 0;

  Config(const tftp::Mode& mode_, const struct PortRange& port_range_,
         bool literal_mode_, const Hostname& hostname_, Seconds timeout_,
         Seconds rexmt_timeout_)
      : mode(mode_),
        ports(port_range_),
        literal_mode(literal_mode_),
        hostname(hostname_),
        timeout(timeout_),
        rexmt_timeout(rexmt_timeout_) {}
};

}  // namespace client
}  // namespace tftp

#endif
