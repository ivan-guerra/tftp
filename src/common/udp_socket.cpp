#include "common/udp_socket.h"

#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cerrno>
#include <cstdint>
#include <cstring>
#include <expected>
#include <string>
#include <string_view>

namespace tftp {

UdpSocketRecver::~UdpSocketRecver() {
  if (-1 != socket_) {
    close(socket_);
  }

  socket_ = -1;
  port_ = 0;
  last_sender_port_ = 0;
}

UdpSocketRecver::UdpSocketRecver(UdpSocketRecver&& other) : UdpSocketRecver() {
  Swap(*this, other);
}

UdpSocketRecver& UdpSocketRecver::operator=(UdpSocketRecver&& other) {
  UdpSocketRecver tmp(std::move(other));
  Swap(*this, tmp);
  return *this;
}

std::expected<UdpSocketRecver, UdpSocketErr> UdpSocketRecver::Create(
    uint16_t port, uint32_t timeout_ms) {
  struct addrinfo* servinfo = nullptr;
  struct addrinfo hints = {};
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_flags = AI_PASSIVE; /* Use this machine's IP. */

  int retcode =
      getaddrinfo(nullptr, std::to_string(port).c_str(), &hints, &servinfo);
  if (retcode == -1) {
    return std::unexpected(std::strerror(errno));
  } else if (retcode) {
    return std::unexpected(gai_strerror(retcode));
  }

  /* Bind to the first address we can. */
  int sockfd = 0;
  struct addrinfo* it = nullptr;
  for (it = servinfo; it; it = it->ai_next) {
    sockfd = socket(it->ai_family, it->ai_socktype, it->ai_protocol);
    if (-1 == sockfd) {
      continue;
    }

    if (bind(sockfd, it->ai_addr, it->ai_addrlen) == -1) {
      close(sockfd);
      continue;
    }

    break; /* We were able to construct and bind a socket. */
  }

  if (!it) {
    return std::unexpected("failed to bind socket");
  }
  freeaddrinfo(servinfo);

  /* Don't wait for the kernel to release the resource. Allow port reuse. */
  int yes = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
    close(sockfd);
    return std::unexpected(std::strerror(errno));
  }

  /* Set a timeout on the socket if one was requested. */
  if (timeout_ms) {
    struct timeval tv = {.tv_sec = 1000 * timeout_ms / 1000000,
                         .tv_usec = 1000 * timeout_ms % 1000000};
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,
                   reinterpret_cast<const char*>(&tv), sizeof(tv)) == -1) {
      close(sockfd);
      return std::unexpected(std::strerror(errno));
    }
  }

  return UdpSocketRecver(sockfd, port);
}

std::expected<ssize_t, UdpSocketErr> UdpSocketRecver::Recv(void* buffer,
                                                           std::size_t len) {
  struct sockaddr_storage sender_addr = {};
  socklen_t addr_len = sizeof(sender_addr);
  ssize_t num_bytes =
      recvfrom(socket_, reinterpret_cast<char*>(buffer), len, 0,
               reinterpret_cast<struct sockaddr*>(&sender_addr), &addr_len);
  if (-1 == num_bytes) {
    if (EAGAIN == errno) { /* This means we just timedout with no data. */
      num_bytes = 0;
    } else { /* Something actually went wrong. */
      return std::unexpected(std::strerror(errno));
    }
  }

  /* Save the port of the sender. */
  if (sender_addr.ss_family == AF_INET6) {
    last_sender_port_ =
        ntohs(reinterpret_cast<sockaddr_in6*>(&sender_addr)->sin6_port);
  } else if (sender_addr.ss_family == AF_INET) {
    last_sender_port_ =
        ntohs(reinterpret_cast<sockaddr_in*>(&sender_addr)->sin_port);
  } else {
    std::unexpected("protocol is not IPv4 or IPv6");
  }

  return num_bytes;
}

void Swap(UdpSocketRecver& r1, UdpSocketRecver& r2) {
  using std::swap;

  swap(r1.socket_, r2.socket_);
  swap(r1.port_, r2.port_);
  swap(r1.last_sender_port_, r2.last_sender_port_);
}

UdpSocketSender::~UdpSocketSender() {
  if (-1 != socket_) {
    close(socket_);
  }

  if (servinfo_) {
    freeaddrinfo(servinfo_);
  }

  socket_ = -1;
  ip_addr_ = "";
  port_ = 0;
  servinfo_ = nullptr;
  addr_ = nullptr;
}

UdpSocketSender::UdpSocketSender(UdpSocketSender&& other) : UdpSocketSender() {
  Swap(*this, other);
}

UdpSocketSender& UdpSocketSender::operator=(UdpSocketSender&& other) {
  UdpSocketSender tmp(std::move(other));
  Swap(*this, tmp);
  return *this;
}

std::expected<UdpSocketSender, UdpSocketErr> UdpSocketSender::Create(
    std::string_view ip_addr, uint16_t port) {
  struct addrinfo* servinfo = nullptr;
  struct addrinfo hints = {};
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_DGRAM;

  int retcode = getaddrinfo(ip_addr.data(), std::to_string(port).c_str(),
                            &hints, &servinfo);
  if (retcode == -1) {
    return std::unexpected(std::strerror(errno));
  } else if (retcode) {
    return std::unexpected(gai_strerror(retcode));
  }

  /* Create a socket from the first valid address. */
  int sockfd = 0;
  struct addrinfo* it = nullptr;
  for (it = servinfo; it; it = it->ai_next) {
    sockfd = socket(it->ai_family, it->ai_socktype, it->ai_protocol);
    if (-1 == sockfd) {
      continue;
    }
    break;
  }

  if (!it) {
    freeaddrinfo(servinfo);
    return std::unexpected("failed to create socket");
  }

  /* Don't wait for the kernel to release the resource. Allow port reuse. */
  int yes = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
    close(sockfd);
    freeaddrinfo(servinfo);
    return std::unexpected(std::strerror(errno));
  }

  return UdpSocketSender(sockfd, ip_addr, port, servinfo, it);
}

std::expected<ssize_t, UdpSocketErr> UdpSocketSender::Send(void* buffer,
                                                           std::size_t len) {
  ssize_t num_bytes = sendto(socket_, reinterpret_cast<char*>(buffer), len, 0,
                             addr_->ai_addr, addr_->ai_addrlen);
  if (-1 == num_bytes) {
    return std::unexpected(std::strerror(errno));
  }
  return num_bytes;
}

void Swap(UdpSocketSender& r1, UdpSocketSender& r2) {
  using std::swap;

  swap(r1.socket_, r2.socket_);
  swap(r1.ip_addr_, r2.ip_addr_);
  swap(r1.port_, r2.port_);
  swap(r1.servinfo_, r2.servinfo_);
  swap(r1.addr_, r2.addr_);
}

}  // namespace tftp
