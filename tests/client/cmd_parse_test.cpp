#include <gtest/gtest.h>

#include <algorithm>
#include <string>

#include "client/cmd.h"

TEST(CmdParseTest, CreateValidConnectCmdWithoutPortReturnsSuccess) {
  std::string cmdline = "connect localhost";
  auto conn_cmd = tftp::client::ConnectCmd::Create(cmdline);

  ASSERT_TRUE(conn_cmd);
  ASSERT_EQ(conn_cmd->Host(), "localhost");
  ASSERT_EQ(conn_cmd->Port(), 0);
}

TEST(CmdParseTest, CreateValidConnectCmdWithPortReturnsSuccess) {
  std::string cmdline = "connect localhost 5555";
  auto conn_cmd = tftp::client::ConnectCmd::Create(cmdline);

  ASSERT_TRUE(conn_cmd);
  ASSERT_EQ(conn_cmd->Host(), "localhost");
  ASSERT_EQ(conn_cmd->Port(), 5555);
}

TEST(CmdParseTest, CreateConnectCmdWithInvalidArgCountReturnsInvalidNumArgs) {
  std::string too_many_args = "connect localhost 5555 foo";
  std::string too_few_args = "connect";
  auto conn_cmd_too_many_args = tftp::client::ConnectCmd::Create(too_many_args);
  auto conn_cmd_too_few_args = tftp::client::ConnectCmd::Create(too_few_args);

  ASSERT_FALSE(conn_cmd_too_many_args);
  ASSERT_FALSE(conn_cmd_too_few_args);
  ASSERT_EQ(conn_cmd_too_many_args.error(),
            tftp::client::ParseStatus::kInvalidNumArgs);
  ASSERT_EQ(conn_cmd_too_few_args.error(),
            tftp::client::ParseStatus::kInvalidNumArgs);
}

TEST(CmdParseTest, CreateConnectCmdWithNegativePortReturnsInvalidPortNum) {
  std::string cmdline = "connect localhost -5555";
  auto conn_cmd = tftp::client::ConnectCmd::Create(cmdline);

  ASSERT_FALSE(conn_cmd);
  ASSERT_EQ(conn_cmd.error(), tftp::client::ParseStatus::kInvalidPortNum);
}

TEST(CmdParseTest, CreateConnectCmdWithNonNumericPortReturnsInvalidPortNum) {
  std::string cmdline = "connect localhost foo";
  auto conn_cmd = tftp::client::ConnectCmd::Create(cmdline);

  ASSERT_FALSE(conn_cmd);
  ASSERT_EQ(conn_cmd.error(), tftp::client::ParseStatus::kInvalidPortNum);
}

TEST(CmdParseTest, CreateConnectCmdWithOutOfRangePortReturnsInvalidPortNum) {
  std::string cmdline = "connect localhost 65536";
  auto conn_cmd = tftp::client::ConnectCmd::Create(cmdline);

  ASSERT_FALSE(conn_cmd);
  ASSERT_EQ(conn_cmd.error(), tftp::client::ParseStatus::kInvalidPortNum);
}

TEST(CmdParseTest, CreateTimeoutWithValidTimeArgReturnsSuccess) {
  std::string cmdline = "timeout 1234";
  auto timeout_cmd = tftp::client::TimeoutCmd::Create(cmdline);

  ASSERT_TRUE(timeout_cmd);
  ASSERT_EQ(timeout_cmd->Timeout(), 1234);
}

TEST(CmdParseTest, CreateTimeoutWithInvalidArgCountReturnsInvalidNumArgs) {
  std::string too_many_args = "timeout 1234 foo";
  std::string too_few_args = "timeout";
  auto timeout_cmd_too_many_args =
      tftp::client::TimeoutCmd::Create(too_many_args);
  auto timeout_cmd_too_few_args =
      tftp::client::TimeoutCmd::Create(too_few_args);

  ASSERT_FALSE(timeout_cmd_too_many_args);
  ASSERT_FALSE(timeout_cmd_too_few_args);
  ASSERT_EQ(timeout_cmd_too_many_args.error(),
            tftp::client::ParseStatus::kInvalidNumArgs);
  ASSERT_EQ(timeout_cmd_too_few_args.error(),
            tftp::client::ParseStatus::kInvalidNumArgs);
}

TEST(CmdParseTest, CreateTimeoutWithNonNumericArgReturnsInvalidTimeout) {
  std::string cmdline = "timeout foo";
  auto timeout_cmd = tftp::client::TimeoutCmd::Create(cmdline);

  ASSERT_FALSE(timeout_cmd);
  ASSERT_EQ(timeout_cmd.error(), tftp::client::ParseStatus::kInvalidTimeout);
}

TEST(CmdParseTest, CreateTimeoutWithNegativeArgReturnsInvalidTimeout) {
  std::string cmdline = "timeout -1";
  auto timeout_cmd = tftp::client::TimeoutCmd::Create(cmdline);

  ASSERT_FALSE(timeout_cmd);
  ASSERT_EQ(timeout_cmd.error(), tftp::client::ParseStatus::kInvalidTimeout);
}

TEST(CmdParseTest, CreateTimeoutWithOutOfRangeArgReturnsInvalidTimeout) {
  std::string cmdline = "timeout 4294967296";
  auto timeout_cmd = tftp::client::TimeoutCmd::Create(cmdline);

  ASSERT_FALSE(timeout_cmd);
  ASSERT_EQ(timeout_cmd.error(), tftp::client::ParseStatus::kInvalidTimeout);
}

TEST(CmdParseTest, CreateRexmtWithValidTimeArgReturnsSuccess) {
  std::string cmdline = "rexmt 1234";
  auto timeout_cmd = tftp::client::RexmtCmd::Create(cmdline);

  ASSERT_TRUE(timeout_cmd);
  ASSERT_EQ(timeout_cmd->RexmtTimeout(), 1234);
}

TEST(CmdParseTest, CreateRexmtWithInvalidArgCountReturnsInvalidNumArgs) {
  std::string too_many_args = "timeout 1234 foo";
  std::string too_few_args = "timeout";
  auto timeout_cmd_too_many_args =
      tftp::client::RexmtCmd::Create(too_many_args);
  auto timeout_cmd_too_few_args = tftp::client::RexmtCmd::Create(too_few_args);

  ASSERT_FALSE(timeout_cmd_too_many_args);
  ASSERT_FALSE(timeout_cmd_too_few_args);
  ASSERT_EQ(timeout_cmd_too_many_args.error(),
            tftp::client::ParseStatus::kInvalidNumArgs);
  ASSERT_EQ(timeout_cmd_too_few_args.error(),
            tftp::client::ParseStatus::kInvalidNumArgs);
}

TEST(CmdParseTest, CreateRexmtWithNonNumericArgReturnsInvalidTimeout) {
  std::string cmdline = "rexmt foo";
  auto timeout_cmd = tftp::client::RexmtCmd::Create(cmdline);

  ASSERT_FALSE(timeout_cmd);
  ASSERT_EQ(timeout_cmd.error(), tftp::client::ParseStatus::kInvalidTimeout);
}

TEST(CmdParseTest, CreateRexmtWithNegativeArgReturnsInvalidTimeout) {
  std::string cmdline = "rexmt -1";
  auto timeout_cmd = tftp::client::RexmtCmd::Create(cmdline);

  ASSERT_FALSE(timeout_cmd);
  ASSERT_EQ(timeout_cmd.error(), tftp::client::ParseStatus::kInvalidTimeout);
}

TEST(CmdParseTest, CreateRexmtWithOutOfRangeArgReturnsInvalidTimeout) {
  std::string cmdline = "rexmt 4294967296";
  auto timeout_cmd = tftp::client::RexmtCmd::Create(cmdline);

  ASSERT_FALSE(timeout_cmd);
  ASSERT_EQ(timeout_cmd.error(), tftp::client::ParseStatus::kInvalidTimeout);
}

TEST(CmdParseTest, CreateHelpCmdWithValidArgsReturnsSuccess) {
  std::string cmdline = "help foo";
  auto help_cmd = tftp::client::HelpCmd::Create(cmdline);

  ASSERT_TRUE(help_cmd);
  ASSERT_EQ(help_cmd->TargetCmd(), "foo");
}

TEST(CmdParseTest, CreateHelpCmdWithInvalidArgCountReturnsInvalidNumArgs) {
  std::string too_many_args = "help foo bar";
  std::string too_few_args = "help";
  auto help_cmd_too_many_args = tftp::client::HelpCmd::Create(too_many_args);
  auto help_cmd_too_few_args = tftp::client::HelpCmd::Create(too_few_args);

  ASSERT_FALSE(help_cmd_too_many_args);
  ASSERT_FALSE(help_cmd_too_few_args);
  ASSERT_EQ(help_cmd_too_many_args.error(),
            tftp::client::ParseStatus::kInvalidNumArgs);
  ASSERT_EQ(help_cmd_too_few_args.error(),
            tftp::client::ParseStatus::kInvalidNumArgs);
}

TEST(CmdParseTest, CreateGetCmdWithInvalidArgCountReturnsInvalidNumArgs) {
  std::string too_few_args = "get";
  auto get_cmd = tftp::client::GetCmd::Create(too_few_args);

  ASSERT_FALSE(get_cmd);
  ASSERT_EQ(get_cmd.error(), tftp::client::ParseStatus::kInvalidNumArgs);
}

TEST(CmdParseTest, CreateGetCmdSingleFileReturnsSuccess) {
  std::string cmdline = "get foo";
  auto get_cmd = tftp::client::GetCmd::Create(cmdline);

  ASSERT_TRUE(get_cmd);
  ASSERT_TRUE(get_cmd->LocalFile().empty());
  ASSERT_TRUE(get_cmd->RemoteFile().empty());
  ASSERT_EQ(get_cmd->Files().size(), 1);
  ASSERT_EQ(get_cmd->Files()[0], "foo");
}

TEST(CmdParseTest, CreateGetCmdWithRemoteAndLocalFileReturnsSuccess) {
  tftp::client::File remote_file = "localhost:/remote/file";
  tftp::client::File local_file = "/local/file";
  std::string cmdline = "get " + remote_file + " " + local_file;
  auto get_cmd = tftp::client::GetCmd::Create(cmdline);

  ASSERT_TRUE(get_cmd);
  ASSERT_EQ(get_cmd->LocalFile(), local_file);
  ASSERT_EQ(get_cmd->RemoteFile(), remote_file);
  ASSERT_TRUE(get_cmd->Files().empty());
}

TEST(CmdParseTest, CreateGetCmdMultiFileReturnsSuccess) {
  std::string cmdline = "get";
  const tftp::client::FileList kFiles = {"foo", "bar", "baz", "gaz"};
  std::for_each(kFiles.cbegin(), kFiles.cend(),
                [&cmdline](const auto& f) { cmdline += " " + f; });

  auto get_cmd = tftp::client::GetCmd::Create(cmdline);
  ASSERT_TRUE(get_cmd);
  ASSERT_TRUE(get_cmd->LocalFile().empty());
  ASSERT_TRUE(get_cmd->RemoteFile().empty());

  ASSERT_EQ(get_cmd->Files().size(), kFiles.size());
  for (std::size_t i = 0; i < kFiles.size(); ++i) {
    ASSERT_EQ(get_cmd->Files()[i], kFiles[i]);
  }
}

TEST(CmdParseTest, CreatePutCmdWithInvalidArgCountReturnsInvalidNumArgs) {
  std::string too_few_args = "get";
  auto put_cmd = tftp::client::PutCmd::Create(too_few_args);

  ASSERT_FALSE(put_cmd);
  ASSERT_EQ(put_cmd.error(), tftp::client::ParseStatus::kInvalidNumArgs);
}

TEST(CmdParseTest, CreatePutCmdSingleFileReturnsSuccess) {
  std::string cmdline = "put foo";
  auto put_cmd = tftp::client::PutCmd::Create(cmdline);

  ASSERT_TRUE(put_cmd);
  ASSERT_TRUE(put_cmd->LocalFile().empty());
  ASSERT_TRUE(put_cmd->RemoteFile().empty());
  ASSERT_EQ(put_cmd->Files().size(), 1);
  ASSERT_EQ(put_cmd->Files()[0], "foo");
}

TEST(CmdParseTest, CreatePutCmdWithLocalAndRemoteFileReturnsSuccess) {
  tftp::client::File local_file = "/local/file";
  tftp::client::File remote_file = "localhost:/remote/file";
  std::string cmdline = "put " + local_file + " " + remote_file;
  auto put_cmd = tftp::client::PutCmd::Create(cmdline);

  ASSERT_TRUE(put_cmd);
  ASSERT_EQ(put_cmd->LocalFile(), local_file);
  ASSERT_EQ(put_cmd->RemoteFile(), remote_file);
  ASSERT_TRUE(put_cmd->Files().empty());
}

TEST(CmdParseTest, CreatePutCmdMultiFileAndRemoteDirReturnsSuccess) {
  std::string cmdline = "put";
  const tftp::client::FileList kFiles = {"foo", "bar", "baz", "gaz",
                                         "/remote/dir"};
  std::for_each(kFiles.cbegin(), kFiles.cend(),
                [&cmdline](const auto& f) { cmdline += " " + f; });

  auto put_cmd = tftp::client::PutCmd::Create(cmdline);
  ASSERT_TRUE(put_cmd);
  ASSERT_TRUE(put_cmd->LocalFile().empty());
  ASSERT_TRUE(put_cmd->RemoteFile().empty());

  ASSERT_EQ(put_cmd->Files().size(), kFiles.size() - 1);
  for (std::size_t i = 0; i < (kFiles.size() - 1); ++i) {
    ASSERT_EQ(put_cmd->Files()[i], kFiles[i]);
  }
  ASSERT_EQ(put_cmd->RemoteDir(), kFiles.back());
}
