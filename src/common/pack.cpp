#include "common/pack.h"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string_view>

#include "common/types.h"

namespace tftp {

static void PackUint16(uint16_t value, std::size_t offset, TftpPacket& packet) {
  packet[offset] = value >> 8;
  packet[offset + 1] = value & 0xFF;
}

static void PackStr(std::string_view str, std::size_t offset,
                    TftpPacket& packet) {
  std::copy(str.cbegin(), str.cend(), packet.begin() + offset);
  packet[offset + str.size()] = 0; /* Append the null terminator. */
}

static void PackData(const BlockData& data, std::size_t offset,
                     TftpPacket& packet) {
  std::copy(data.cbegin(), data.cend(), packet.begin() + offset);
}

static TftpPacket PackRequest(OpCode req_code, std::string_view filename,
                              Mode mode) {
  std::size_t packet_len =
      sizeof(req_code) + (filename.size() + 1) + (mode.size() + 1);
  TftpPacket packet(packet_len, 0);

  std::size_t offset = 0;
  PackUint16(req_code, offset, packet);
  offset += sizeof(req_code);

  PackStr(filename, offset, packet);
  offset += filename.size() + 1;

  PackStr(mode, offset, packet);

  return packet;
}

static std::optional<uint16_t> UnpackUint16(const TftpPacket& packet,
                                            std::size_t offset) {
  if ((offset + sizeof(uint16_t)) > packet.size()) {
    return std::nullopt;
  }
  return (packet[offset] << 8) | packet[offset + 1];
}

static std::optional<std::string> UnpackStr(const TftpPacket& packet,
                                            std::size_t offset) {
  std::size_t i = offset;
  std::string str;
  for (; i < packet.size() && packet[i]; ++i) {
    str += static_cast<char>(packet[i]);
  }
  /* Check if the null terminator was found. */
  return (i == packet.size()) ? std::optional<std::string>{} : str;
}

static bool IsValidMode(std::string_view candidate) {
  return ((candidate == SendMode::kNetAscii) ||
          (candidate == SendMode::kOctet) || (candidate == SendMode::kMail));
}

static bool IsValidErrCode(uint16_t err_code) {
  return ((err_code >= ErrorCode::kNotDefined) &&
          (err_code <= ErrorCode::kNoSuchUser));
}

TftpPacket PackReadRequest(const ReadRequestMsg& msg) {
  return PackRequest(msg.op, msg.filename, msg.mode);
}

TftpPacket PackWriteRequest(const WriteRequestMsg& msg) {
  return PackRequest(msg.op, msg.filename, msg.mode);
}

TftpPacket PackData(const DataMsg& msg) {
  std::size_t packet_len =
      sizeof(msg.op) + sizeof(msg.block_num) + msg.data.size();
  TftpPacket packet(packet_len, 0);

  std::size_t offset = 0;
  PackUint16(msg.op, offset, packet);
  offset += sizeof(msg.op);

  PackUint16(msg.block_num, offset, packet);
  offset += sizeof(msg.block_num);

  PackData(msg.data, offset, packet);

  return packet;
}

TftpPacket PackAck(const AckMsg& msg) {
  std::size_t packet_len = sizeof(msg.op) + sizeof(msg.block_num);
  TftpPacket packet(packet_len, 0);

  std::size_t offset = 0;
  PackUint16(msg.op, offset, packet);
  offset += sizeof(msg.op);

  PackUint16(msg.block_num, offset, packet);

  return packet;
}

TftpPacket PackError(const ErrorMsg& msg) {
  std::size_t packet_len =
      sizeof(msg.op) + sizeof(msg.err_code) + (msg.err_msg.size() + 1);
  TftpPacket packet(packet_len, 0);

  std::size_t offset = 0;
  PackUint16(OpCode::kError, offset, packet);
  offset += sizeof(msg.op);

  PackUint16(msg.err_code, offset, packet);
  offset += sizeof(msg.err_code);

  PackStr(msg.err_msg, offset, packet);

  return packet;
}

std::optional<ReadRequestMsg> UnpackReadRequest(const TftpPacket& packet) {
  std::size_t offset = 0;
  auto opcode = UnpackUint16(packet, offset);
  if (!opcode || *opcode != OpCode::kReadReq) {
    return std::nullopt;
  }
  offset += sizeof(*opcode);

  auto filename = UnpackStr(packet, offset);
  if (!filename) {
    return std::nullopt;
  }
  offset += filename->size() + 1;

  auto mode = UnpackStr(packet, offset);
  if (!mode || !IsValidMode(*mode)) {
    return std::nullopt;
  }

  return std::optional<ReadRequestMsg>(
      {.op = OpCode::kReadReq, .filename = *filename, .mode = *mode});
}

std::optional<WriteRequestMsg> UnpackWriteRequest(const TftpPacket& packet) {
  std::size_t offset = 0;
  auto opcode = UnpackUint16(packet, offset);
  if (!opcode || *opcode != OpCode::kWriteReq) {
    return std::nullopt;
  }
  offset += sizeof(*opcode);

  auto filename = UnpackStr(packet, offset);
  if (!filename) {
    return std::nullopt;
  }
  offset += filename->size() + 1;

  auto mode = UnpackStr(packet, offset);
  if (!mode || !IsValidMode(*mode)) {
    return std::nullopt;
  }

  return std::optional<WriteRequestMsg>(
      {.op = OpCode::kWriteReq, .filename = *filename, .mode = *mode});
}

std::optional<DataMsg> UnpackData(const TftpPacket& packet) {
  std::size_t offset = 0;
  auto opcode = UnpackUint16(packet, offset);
  if (!opcode || *opcode != OpCode::kData) {
    return std::nullopt;
  }
  offset += sizeof(*opcode);

  auto block_num = UnpackUint16(packet, offset);
  if (!block_num) {
    return std::nullopt;
  }
  offset += sizeof(*block_num);

  DataMsg data_msg;
  data_msg.block_num = *block_num;
  data_msg.data.insert(data_msg.data.begin(), packet.cbegin() + offset,
                       packet.cend());

  return data_msg;
}

std::optional<AckMsg> UnpackAck(const TftpPacket& packet) {
  std::size_t offset = 0;
  auto opcode = UnpackUint16(packet, offset);
  if (!opcode || *opcode != OpCode::kAck) {
    return std::nullopt;
  }
  offset += sizeof(*opcode);

  auto block_num = UnpackUint16(packet, offset);
  if (!block_num) {
    return std::nullopt;
  }

  return std::optional<AckMsg>({.op = OpCode::kAck, .block_num = *block_num});
}

std::optional<ErrorMsg> UnpackError(const TftpPacket& packet) {
  std::size_t offset = 0;
  auto opcode = UnpackUint16(packet, offset);
  if (!opcode || *opcode != OpCode::kError) {
    return std::nullopt;
  }
  offset += sizeof(*opcode);

  auto err_code = UnpackUint16(packet, offset);
  if (!err_code || !IsValidErrCode(*err_code)) {
    return std::nullopt;
  }
  offset += sizeof(*err_code);

  auto err_msg = UnpackStr(packet, offset);
  if (!err_msg) {
    return std::nullopt;
  }

  return std::optional<ErrorMsg>({.op = OpCode::kError,
                                  .err_code = static_cast<ErrorCode>(*err_code),
                                  .err_msg = *err_msg});
}

}  // namespace tftp
