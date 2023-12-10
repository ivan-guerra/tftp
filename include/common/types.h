#ifndef TYPES_H_
#define TYPES_H_

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace tftp {

struct Packet; /* forward declaration to allow 'using' definition below */

using ByteVec = std::vector<uint8_t>;

namespace mode {
static const char* kNetAscii = "netascii";
static const char* kOctet = "octet";
static const char* kMail = "mail";
}  // namespace mode

enum OpCode : uint16_t {
  kReadRequest = 1,
  kWriteRequest,
  kData,
  kAck,
  kError,
};

enum ErrorCode : uint16_t {
  kNotDefined = 0,
  kFileNotFound,
  kAccessViolation,
  kDiskFull,
  kIllegalOperation,
  kUnknownTransferId,
  kFileAreadyExists,
  kNoSuchUser,
};

struct RequestPacket {
  uint16_t opcode;
  std::string filename;
  std::string mode;
};

struct DataPacket {
  uint16_t opcode;
  uint16_t block_num = 0;
  ByteVec data;
};

struct AckPacket {
  uint16_t opcode;
  uint16_t block_num = 0;
};

struct ErrorPacket {
  uint16_t opcode;
  uint16_t error_code;
  std::string msg;
};

}  // namespace tftp

#endif
