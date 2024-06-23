#ifndef TYPES_H_
#define TYPES_H_

#include <cstdint>
#include <string>
#include <vector>

namespace tftp {

using BlockNum = uint16_t;
using BlockData = std::vector<uint8_t>;
using TftpPacket = std::vector<uint8_t>;
using Mode = std::string;
using Hostname = std::string;
using Seconds = uint16_t;

struct PortRange {
  uint16_t start = 0;
  uint16_t end = 0;
};

enum ErrorCode : uint16_t {
  kNotDefined = 0,
  kFileNotFound,
  kAccessViolation,
  kDiskFullOrAllocExceeded,
  kIllegalOperation,
  kUnknownTransferId,
  kFileAlreadyExists,
  kNoSuchUser,
};

enum OpCode : uint16_t {
  kReadReq = 1,
  kWriteReq,
  kData,
  kAck,
  kError,
};

namespace SendMode {
constexpr Mode kNetAscii = "netascii";
constexpr Mode kOctet = "octet";
constexpr Mode kMail = "mail";
}  // namespace SendMode

struct ReadRequestMsg {
  OpCode op = OpCode::kReadReq;
  std::string filename;
  Mode mode;
};

struct WriteRequestMsg {
  OpCode op = OpCode::kWriteReq;
  std::string filename;
  Mode mode;
};

struct DataMsg {
  OpCode op = OpCode::kData;
  BlockNum block_num = 0;
  BlockData data;
};

struct AckMsg {
  OpCode op = OpCode::kAck;
  BlockNum block_num = 0;
};

struct ErrorMsg {
  OpCode op = OpCode::kError;
  ErrorCode err_code;
  std::string err_msg;
};

};  // namespace tftp

#endif
