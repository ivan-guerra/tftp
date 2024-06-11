#include "common/pack.h"

#include <gtest/gtest.h>

#include "common/types.h"

TEST(CommonTest, PackReadRequestReturnsValidPacket) {
  tftp::ReadRequestMsg rrq = {.op = tftp::OpCode::kReadReq,
                              .filename = "test",
                              .mode = tftp::SendMode::kNetAscii};

  tftp::TftpPacket actual_packet = tftp::PackReadRequest(rrq);
  tftp::TftpPacket expected_packet = {0x0, 0x1, 't', 'e', 's', 't', 0x0, 'n',
                                      'e', 't', 'a', 's', 'c', 'i', 'i', 0x0};

  ASSERT_EQ(actual_packet, expected_packet);
}

TEST(CommonTest, PackWriteRequestReturnsValidPacket) {
  tftp::WriteRequestMsg wrq = {.op = tftp::OpCode::kWriteReq,
                               .filename = "test",
                               .mode = tftp::SendMode::kNetAscii};

  tftp::TftpPacket actual_packet = tftp::PackWriteRequest(wrq);
  tftp::TftpPacket expected_packet = {0x0, 0x2, 't', 'e', 's', 't', 0x0, 'n',
                                      'e', 't', 'a', 's', 'c', 'i', 'i', 0x0};

  ASSERT_EQ(actual_packet, expected_packet);
}

TEST(CommonTest, PackDataReturnsValidPacket) {
  tftp::DataMsg data = {
      .op = tftp::OpCode::kData, .block_num = 42, .data = {'f', 'o', 'o'}};

  tftp::TftpPacket actual_packet = tftp::PackData(data);
  tftp::TftpPacket expected_packet = {0x0, 0x3, 0x0, 0x2A, 'f', 'o', 'o'};

  ASSERT_EQ(actual_packet, expected_packet);
}

TEST(CommonTest, PackAckReturnsValidPacket) {
  tftp::AckMsg ack = {.op = tftp::OpCode::kAck, .block_num = 42};

  tftp::TftpPacket actual_packet = tftp::PackAck(ack);
  tftp::TftpPacket expected_packet = {0x0, 0x4, 0x0, 0x2A};

  ASSERT_EQ(actual_packet, expected_packet);
}

TEST(CommonTest, PackErrorReturnsValidPacket) {
  tftp::ErrorMsg err = {.op = tftp::OpCode::kError,
                        .err_code = tftp::ErrorCode::kNoSuchUser,
                        .err_msg = "foo"};

  tftp::TftpPacket actual_packet = tftp::PackError(err);
  tftp::TftpPacket expected_packet = {0x0, 0x5, 0x0, 0x7, 'f', 'o', 'o', 0x0};

  ASSERT_EQ(actual_packet, expected_packet);
}

TEST(CommonTest, UnpackReadRequestReturnsValidMsg) {
  tftp::TftpPacket rrq_packet = {0x0, 0x1, 't', 'e', 's', 't', 0x0, 'n',
                                 'e', 't', 'a', 's', 'c', 'i', 'i', 0x0};
  auto actual_msg = tftp::UnpackReadRequest(rrq_packet);
  ASSERT_TRUE(actual_msg);

  tftp::ReadRequestMsg expected_msg = {.op = tftp::OpCode::kReadReq,
                                       .filename = "test",
                                       .mode = tftp::SendMode::kNetAscii};
  ASSERT_EQ(actual_msg->op, expected_msg.op);
  ASSERT_EQ(actual_msg->filename, expected_msg.filename);
  ASSERT_EQ(actual_msg->mode, expected_msg.mode);
}

TEST(CommonTest, UnpackReadRequestReturnsNulloptOnInvalidOpCode) {
  tftp::TftpPacket rrq_packet = {0x0, 0xA, 't', 'e', 's', 't', 0x0, 'n',
                                 'e', 't', 'a', 's', 'c', 'i', 'i', 0x0};
  auto actual_msg = tftp::UnpackReadRequest(rrq_packet);

  ASSERT_FALSE(actual_msg);
}

TEST(CommonTest, UnpackReadRequestReturnsNulloptOnUnterminatedFilename) {
  tftp::TftpPacket rrq_packet = {0x0, 0x1, 't', 'e', 's', 't', 'n', 'e',
                                 't', 'a', 's', 'c', 'i', 'i', 0x0};
  auto actual_msg = tftp::UnpackReadRequest(rrq_packet);

  ASSERT_FALSE(actual_msg);
}

TEST(CommonTest, UnpackReadRequestReturnsNulloptOnUnterminatedMode) {
  tftp::TftpPacket rrq_packet = {0x0, 0x1, 't', 'e', 's', 't', 0x0, 'n',
                                 'e', 't', 'a', 's', 'c', 'i', 'i'};
  auto actual_msg = tftp::UnpackReadRequest(rrq_packet);

  ASSERT_FALSE(actual_msg);
}

TEST(CommonTest, UnpackReadRequestReturnsNulloptOnInvalidMode) {
  tftp::TftpPacket rrq_packet = {0x0, 0x1, 't', 'e', 's', 't',
                                 0x0, 'f', 'o', 'o', 0x0};
  auto actual_msg = tftp::UnpackReadRequest(rrq_packet);

  ASSERT_FALSE(actual_msg);
}

TEST(CommonTest, UnpackReadRequestReturnsNulloptOnEmptyPacket) {
  ASSERT_FALSE(tftp::UnpackReadRequest({}));
}

TEST(CommonTest, UnpackWriteRequestReturnsValidMsg) {
  tftp::TftpPacket wrq_packet = {0x0, 0x2, 't', 'e', 's', 't', 0x0, 'n',
                                 'e', 't', 'a', 's', 'c', 'i', 'i', 0x0};
  auto actual_msg = tftp::UnpackWriteRequest(wrq_packet);
  ASSERT_TRUE(actual_msg);

  tftp::WriteRequestMsg expected_msg = {.op = tftp::OpCode::kWriteReq,
                                        .filename = "test",
                                        .mode = tftp::SendMode::kNetAscii};
  ASSERT_EQ(actual_msg->op, expected_msg.op);
  ASSERT_EQ(actual_msg->filename, expected_msg.filename);
  ASSERT_EQ(actual_msg->mode, expected_msg.mode);
}

TEST(CommonTest, UnpackWriteRequestReturnsNulloptOnInvalidOpCode) {
  tftp::TftpPacket wrq_packet = {0x0, 0xA, 't', 'e', 's', 't', 0x0, 'n',
                                 'e', 't', 'a', 's', 'c', 'i', 'i', 0x0};
  auto actual_msg = tftp::UnpackWriteRequest(wrq_packet);

  ASSERT_FALSE(actual_msg);
}

TEST(CommonTest, UnpackWriteRequestReturnsNulloptOnUnterminatedFilename) {
  tftp::TftpPacket wrq_packet = {0x0, 0x2, 't', 'e', 's', 't', 'n', 'e',
                                 't', 'a', 's', 'c', 'i', 'i', 0x0};
  auto actual_msg = tftp::UnpackWriteRequest(wrq_packet);

  ASSERT_FALSE(actual_msg);
}

TEST(CommonTest, UnpackWriteRequestReturnsNulloptOnUnterminatedMode) {
  tftp::TftpPacket wrq_packet = {0x0, 0x2, 't', 'e', 's', 't', 0x0, 'n',
                                 'e', 't', 'a', 's', 'c', 'i', 'i'};
  auto actual_msg = tftp::UnpackWriteRequest(wrq_packet);

  ASSERT_FALSE(actual_msg);
}

TEST(CommonTest, UnpackWriteRequestReturnsNulloptOnInvalidMode) {
  tftp::TftpPacket wrq_packet = {0x0, 0x2, 't', 'e', 's', 't',
                                 0x0, 'f', 'o', 'o', 0x0};
  auto actual_msg = tftp::UnpackWriteRequest(wrq_packet);

  ASSERT_FALSE(actual_msg);
}

TEST(CommonTest, UnpackWriteRequestReturnsNulloptOnEmptyPacket) {
  ASSERT_FALSE(tftp::UnpackWriteRequest({}));
}

TEST(CommonTest, UnpackDataReturnsValidMsg) {
  tftp::TftpPacket data_packet = {0x0, 0x3, 0x0, 0x2A, 'f', 'o', 'o'};
  auto actual_msg = tftp::UnpackData(data_packet);
  ASSERT_TRUE(actual_msg);

  tftp::DataMsg expected_msg = {
      .op = tftp::OpCode::kData, .block_num = 42, .data = {'f', 'o', 'o'}};
  ASSERT_EQ(actual_msg->op, expected_msg.op);
  ASSERT_EQ(actual_msg->block_num, expected_msg.block_num);
  ASSERT_EQ(actual_msg->data, expected_msg.data);
}

TEST(CommonTest, UnpackDataReturnsNulloptOnInvalidOpCode) {
  tftp::TftpPacket data_packet = {0x0, 0xA, 0x0, 0x2A, 'f', 'o', 'o'};
  auto actual_msg = tftp::UnpackData(data_packet);

  ASSERT_FALSE(actual_msg);
}

TEST(CommonTest, UnpackDataReturnsNulloptOnEmptyPacket) {
  ASSERT_FALSE(tftp::UnpackData({}));
}

TEST(CommonTest, UnpackAckReturnsValidMsg) {
  tftp::TftpPacket ack_packet = {0x0, 0x4, 0x0, 0x2A};
  auto actual_msg = tftp::UnpackAck(ack_packet);
  ASSERT_TRUE(actual_msg);

  tftp::AckMsg expected_msg = {.op = tftp::OpCode::kAck, .block_num = 42};
  ASSERT_EQ(actual_msg->op, expected_msg.op);
  ASSERT_EQ(actual_msg->block_num, expected_msg.block_num);
}

TEST(CommonTest, UnpackAckReturnsNulloptOnInvalidOpCode) {
  tftp::TftpPacket ack_packet = {0x0, 0xA, 0x0, 0x2A};
  auto actual_msg = tftp::UnpackAck(ack_packet);

  ASSERT_FALSE(actual_msg);
}

TEST(CommonTest, UnpackAckReturnsNulloptOnEmptyPacket) {
  ASSERT_FALSE(tftp::UnpackAck({}));
}

TEST(CommonTest, UnpackErrorReturnsValidMsg) {
  tftp::TftpPacket err_packet = {0x0, 0x5, 0x0, 0x0, 'f', 'o', 'o', 0x0};
  auto actual_msg = tftp::UnpackError(err_packet);
  ASSERT_TRUE(actual_msg);

  tftp::ErrorMsg expected_msg = {.op = tftp::OpCode::kError,
                                 .err_code = tftp::ErrorCode::kNotDefined,
                                 .err_msg = "foo"};
  ASSERT_EQ(actual_msg->op, expected_msg.op);
  ASSERT_EQ(actual_msg->err_code, expected_msg.err_code);
  ASSERT_EQ(actual_msg->err_msg, expected_msg.err_msg);
}

TEST(CommonTest, UnpackErrorReturnsNulloptOnInvalidOpCode) {
  tftp::TftpPacket err_packet = {0x0, 0xA, 0x0, 0x0, 'f', 'o', 'o', 0x0};
  auto actual_msg = tftp::UnpackError(err_packet);

  ASSERT_FALSE(actual_msg);
}

TEST(CommonTest, UnpackErrorReturnsNulloptOnUnterminatedErrMsg) {
  tftp::TftpPacket err_packet = {0x0, 0x5, 0x0, 0x0, 'f', 'o', 'o'};
  auto actual_msg = tftp::UnpackError(err_packet);

  ASSERT_FALSE(actual_msg);
}

TEST(CommonTest, UnpackErrorReturnsNulloptOnInvalidErrCode) {
  tftp::TftpPacket err_packet = {0x0, 0x5, 0xDE, 0xAD, 'f', 'o', 'o', 0x0};
  auto actual_msg = tftp::UnpackError(err_packet);

  ASSERT_FALSE(actual_msg);
}

TEST(CommonTest, UnpackErrorReturnsNulloptOnEmptyPacket) {
  ASSERT_FALSE(tftp::UnpackError({}));
}
