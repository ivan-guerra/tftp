#ifndef TRANSLATE_H_
#define TRANSLATE_H_

#include "common/types.h"

namespace tftp {

[[nodiscard]] ByteVec PackRequest(const RequestPacket& packet) noexcept;
[[nodiscard]] ByteVec PackData(const DataPacket& packet) noexcept;
[[nodiscard]] ByteVec PackAck(const AckPacket& packet) noexcept;
[[nodiscard]] ByteVec PackError(const ErrorPacket& packet) noexcept;

[[nodiscard]] RequestPacket UnpackRequest(const ByteVec& raw_packet) noexcept;
[[nodiscard]] DataPacket UnpackData(const ByteVec& raw_packet) noexcept;
[[nodiscard]] AckPacket UnpackAck(const ByteVec& raw_packet) noexcept;
[[nodiscard]] ErrorPacket UnpackError(const ByteVec& raw_packet) noexcept;

};  // namespace tftp

#endif
