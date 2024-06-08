#ifndef UDP_SOCKET_H_
#define UDP_SOCKET_H_

#include <netdb.h>
#include <sys/types.h>

#include <cstddef>
#include <cstdint>
#include <expected>
#include <string>
#include <string_view>

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

  std::expected<ssize_t, UdpSocketErr> Recv(void* buffer, std::size_t len);

  friend void Swap(UdpSocketRecver& r1, UdpSocketRecver& r2);

 private:
  explicit UdpSocketRecver(int socket = -1, uint16_t port = 0)
      : socket_(socket), port_(port), last_sender_port_(0) {}

  int socket_ = -1;
  uint16_t port_ = 0;
  uint16_t last_sender_port_ = 0;
};

class UdpSocketSender {
 public:
  static std::expected<UdpSocketSender, UdpSocketErr> Create(
      std::string_view ip_addr, uint16_t port);

  ~UdpSocketSender();
  UdpSocketSender(const UdpSocketSender&) = delete;
  UdpSocketSender& operator=(const UdpSocketSender&) = delete;
  UdpSocketSender(UdpSocketSender&&);
  UdpSocketSender& operator=(UdpSocketSender&&);

  std::string_view IpAddr() const { return ip_addr_; }
  uint16_t SendPort() const { return port_; }

  std::expected<ssize_t, UdpSocketErr> Send(void* buffer, std::size_t len);

  friend void Swap(UdpSocketSender& r1, UdpSocketSender& r2);

 private:
  explicit UdpSocketSender(int socket = -1,
                           std::string_view ip_addr = "127.0.0.1",
                           uint16_t port = 0, addrinfo* servinfo = nullptr,
                           addrinfo* addr = nullptr)
      : socket_(socket),
        ip_addr_(ip_addr),
        port_(port),
        servinfo_(servinfo),
        addr_(addr) {}

  int socket_ = -1;
  std::string ip_addr_;
  uint16_t port_ = 0;
  addrinfo* servinfo_ = nullptr;
  addrinfo* addr_ = nullptr;
};

}  // namespace tftp

#endif
