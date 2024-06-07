#ifndef UDP_SOCKET_H_
#define UDP_SOCKET_H_

#include <sys/types.h>

#include <cstddef>
#include <cstdint>
#include <expected>
#include <string>

namespace tftp {

using UdpSocketErr = std::string;

class UdpSocketRecver {
 public:
  static std::expected<UdpSocketRecver, UdpSocketErr> Create(
      uint16_t port, uint32_t timeout_ms = 0);

  ~UdpSocketRecver();
  UdpSocketRecver(const UdpSocketRecver&) = delete;
  UdpSocketRecver& operator=(const UdpSocketRecver&) = delete;
  UdpSocketRecver(UdpSocketRecver&&);
  UdpSocketRecver& operator=(UdpSocketRecver&&);

  uint16_t RecvPort() const { return port_; }
  uint16_t LastSenderPort() const { return last_sender_port_; }

  std::expected<ssize_t, UdpSocketErr> RecvFrom(void* buffer, std::size_t len);

  friend void Swap(UdpSocketRecver& r1, UdpSocketRecver& r2);

 private:
  explicit UdpSocketRecver(int socket = -1, uint16_t port = 0)
      : socket_(socket), port_(port), last_sender_port_(0) {}

  int socket_ = -1;
  uint16_t port_ = 0;
  uint16_t last_sender_port_ = 0;
};

}  // namespace tftp

#endif
