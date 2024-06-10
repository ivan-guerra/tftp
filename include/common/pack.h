#ifndef PACK_H_
#define PACK_H_

#include <optional>

#include "common/types.h"

namespace tftp {

TftpPacket PackReadRequest(const ReadRequestMsg& msg);
TftpPacket PackWriteRequest(const WriteRequestMsg& msg);
TftpPacket PackData(const DataMsg& msg);
TftpPacket PackAck(const AckMsg& msg);
TftpPacket PackError(const ErrorMsg& msg);

std::optional<ReadRequestMsg> UnpackReadRequest(const TftpPacket& packet);
std::optional<WriteRequestMsg> UnpackWriteRequest(const TftpPacket& packet);
std::optional<DataMsg> UnpackData(const TftpPacket& packet);
std::optional<AckMsg> UnpackAck(const TftpPacket& packet);
std::optional<ErrorMsg> UnpackError(const TftpPacket& packet);

}  // namespace tftp

#endif
