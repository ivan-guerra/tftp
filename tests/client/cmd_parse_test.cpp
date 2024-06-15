#include <gtest/gtest.h>

#include <algorithm>
#include <string>

#include "client/client_cmd.h"

TEST(CmdParseTest, ParseValidConnectCmdWithoutPortReturnsSuccess) {
  std::string cmdline = "connect localhost";
  tftp::client::ConnectCmd conn_cmd;

  ASSERT_EQ(conn_cmd.Parse(cmdline), tftp::client::ParseStatus::kSuccess);
  ASSERT_EQ(conn_cmd.Host(), "localhost");
  ASSERT_EQ(conn_cmd.Port(), 0);
}

TEST(CmdParseTest, ParseValidConnectCmdWithPortReturnsSuccess) {
  std::string cmdline = "connect localhost 5555";
  tftp::client::ConnectCmd conn_cmd;

  ASSERT_EQ(conn_cmd.Parse(cmdline), tftp::client::ParseStatus::kSuccess);
  ASSERT_EQ(conn_cmd.Host(), "localhost");
  ASSERT_EQ(conn_cmd.Port(), 5555);
}

TEST(CmdParseTest, ParseConnectCmdWithInvalidArgCountReturnsInvalidNumArgs) {
  std::string too_many_args = "connect localhost 5555 foo";
  std::string too_few_args = "connect";
  tftp::client::ConnectCmd conn_cmd;

  ASSERT_EQ(conn_cmd.Parse(too_many_args),
            tftp::client::ParseStatus::kInvalidNumArgs);
  ASSERT_EQ(conn_cmd.Parse(too_few_args),
            tftp::client::ParseStatus::kInvalidNumArgs);
}

TEST(CmdParseTest, ParseConnectCmdWithNegativePortReturnsInvalidPortNum) {
  std::string cmdline = "connect localhost -5555";
  tftp::client::ConnectCmd conn_cmd;

  ASSERT_EQ(conn_cmd.Parse(cmdline),
            tftp::client::ParseStatus::kInvalidPortNum);
}

TEST(CmdParseTest, ParseConnectCmdWithNonNumericPortReturnsInvalidPortNum) {
  std::string cmdline = "connect localhost foo";
  tftp::client::ConnectCmd conn_cmd;

  ASSERT_EQ(conn_cmd.Parse(cmdline),
            tftp::client::ParseStatus::kInvalidPortNum);
}

TEST(CmdParseTest, ParseConnectCmdWithOutOfRangePortReturnsInvalidPortNum) {
  std::string cmdline = "connect localhost 65536";
  tftp::client::ConnectCmd conn_cmd;

  ASSERT_EQ(conn_cmd.Parse(cmdline),
            tftp::client::ParseStatus::kInvalidPortNum);
}

TEST(CmdParseTest, ParseTimeoutWithValidTimeArgReturnsSuccess) {
  std::string cmdline = "timeout 1234";
  tftp::client::TimeoutCmd timeout_cmd;

  ASSERT_EQ(timeout_cmd.Parse(cmdline), tftp::client::ParseStatus::kSuccess);
}

TEST(CmdParseTest, ParseTimeoutWithInvalidArgCountReturnsInvalidNumArgs) {
  std::string too_many_args = "timeout 1234 foo";
  std::string too_few_args = "timeout";
  tftp::client::TimeoutCmd timeout_cmd;

  ASSERT_EQ(timeout_cmd.Parse(too_many_args),
            tftp::client::ParseStatus::kInvalidNumArgs);
  ASSERT_EQ(timeout_cmd.Parse(too_few_args),
            tftp::client::ParseStatus::kInvalidNumArgs);
}

TEST(CmdParseTest, ParseTimeoutWithNonNumericArgReturnsInvalidTimeout) {
  std::string cmdline = "timeout foo";
  tftp::client::TimeoutCmd timeout_cmd;

  ASSERT_EQ(timeout_cmd.Parse(cmdline),
            tftp::client::ParseStatus::kInvalidTimeout);
}

TEST(CmdParseTest, ParseTimeoutWithNegativeArgReturnsInvalidTimeout) {
  std::string cmdline = "timeout -1";
  tftp::client::TimeoutCmd timeout_cmd;

  ASSERT_EQ(timeout_cmd.Parse(cmdline),
            tftp::client::ParseStatus::kInvalidTimeout);
}

TEST(CmdParseTest, ParseTimeoutWithOutOfRangeArgReturnsInvalidTimeout) {
  /* TODO: Should be a better way of getting the max value of the Seconds type
   * +1. */
  std::string cmdline = "timeout 4294967296";
  tftp::client::TimeoutCmd timeout_cmd;

  ASSERT_EQ(timeout_cmd.Parse(cmdline),
            tftp::client::ParseStatus::kInvalidTimeout);
}

TEST(CmdParseTest, ParseRexmtWithValidTimeArgReturnsSuccess) {
  std::string cmdline = "rexmt 1234";
  tftp::client::RexmtCmd timeout_cmd;

  ASSERT_EQ(timeout_cmd.Parse(cmdline), tftp::client::ParseStatus::kSuccess);
}

TEST(CmdParseTest, ParseRexmtWithInvalidArgCountReturnsInvalidNumArgs) {
  std::string too_many_args = "timeout 1234 foo";
  std::string too_few_args = "timeout";
  tftp::client::RexmtCmd timeout_cmd;

  ASSERT_EQ(timeout_cmd.Parse(too_many_args),
            tftp::client::ParseStatus::kInvalidNumArgs);
  ASSERT_EQ(timeout_cmd.Parse(too_few_args),
            tftp::client::ParseStatus::kInvalidNumArgs);
}

TEST(CmdParseTest, ParseRexmtWithNonNumericArgReturnsInvalidTimeout) {
  std::string cmdline = "rexmt foo";
  tftp::client::RexmtCmd timeout_cmd;

  ASSERT_EQ(timeout_cmd.Parse(cmdline),
            tftp::client::ParseStatus::kInvalidTimeout);
}

TEST(CmdParseTest, ParseRexmtWithNegativeArgReturnsInvalidTimeout) {
  std::string cmdline = "rexmt -1";
  tftp::client::RexmtCmd timeout_cmd;

  ASSERT_EQ(timeout_cmd.Parse(cmdline),
            tftp::client::ParseStatus::kInvalidTimeout);
}

TEST(CmdParseTest, ParseRexmtWithOutOfRangeArgReturnsInvalidTimeout) {
  /* TODO: Should be a better way of getting the max value of the Seconds type
   * +1. */
  std::string cmdline = "rexmt 4294967296";
  tftp::client::RexmtCmd timeout_cmd;

  ASSERT_EQ(timeout_cmd.Parse(cmdline),
            tftp::client::ParseStatus::kInvalidTimeout);
}

TEST(CmdParseTest, ParseHelpCmdWithValidArgsReturnsSuccess) {
  std::string cmdline = "help foo";
  tftp::client::HelpCmd help_cmd;

  ASSERT_EQ(help_cmd.Parse(cmdline), tftp::client::ParseStatus::kSuccess);
}

TEST(CmdParseTest, ParseHelpCmdWithInvalidArgCountReturnsInvalidNumArgs) {
  std::string too_many_args = "help foo bar";
  std::string too_few_args = "help";
  tftp::client::HelpCmd help_cmd;

  ASSERT_EQ(help_cmd.Parse(too_many_args),
            tftp::client::ParseStatus::kInvalidNumArgs);
  ASSERT_EQ(help_cmd.Parse(too_few_args),
            tftp::client::ParseStatus::kInvalidNumArgs);
}

TEST(CmdParseTest, ParseGetCmdWithInvalidArgCountReturnsInvalidNumArgs) {
  std::string too_few_args = "get";
  tftp::client::GetCmd get_cmd;

  ASSERT_EQ(get_cmd.Parse(too_few_args),
            tftp::client::ParseStatus::kInvalidNumArgs);
}

TEST(CmdParseTest, ParseGetCmdSingleFileReturnsSuccess) {
  std::string cmdline = "get foo";
  tftp::client::GetCmd get_cmd;

  ASSERT_EQ(get_cmd.Parse(cmdline), tftp::client::ParseStatus::kSuccess);
  ASSERT_TRUE(get_cmd.LocalFile().empty());
  ASSERT_TRUE(get_cmd.RemoteFile().empty());
  ASSERT_EQ(get_cmd.Files().size(), 1);
  ASSERT_EQ(get_cmd.Files()[0], "foo");
}

TEST(CmdParseTest, ParseGetCmdWithRemoteAndLocalFileReturnsSuccess) {
  tftp::client::File remote_file = "localhost:/remote/file";
  tftp::client::File local_file = "/local/file";
  std::string cmdline = "get " + remote_file + " " + local_file;
  tftp::client::GetCmd get_cmd;

  ASSERT_EQ(get_cmd.Parse(cmdline), tftp::client::ParseStatus::kSuccess);
  ASSERT_EQ(get_cmd.LocalFile(), local_file);
  ASSERT_EQ(get_cmd.RemoteFile(), remote_file);
  ASSERT_TRUE(get_cmd.Files().empty());
}

TEST(CmdParseTest, ParseGetCmdMultiFileReturnsSuccess) {
  std::string cmdline = "get";
  const tftp::client::FileList kFiles = {"foo", "bar", "baz", "gaz"};
  std::for_each(kFiles.cbegin(), kFiles.cend(),
                [&cmdline](const auto& f) { cmdline += " " + f; });

  tftp::client::GetCmd get_cmd;
  ASSERT_EQ(get_cmd.Parse(cmdline), tftp::client::ParseStatus::kSuccess);
  ASSERT_TRUE(get_cmd.LocalFile().empty());
  ASSERT_TRUE(get_cmd.RemoteFile().empty());

  ASSERT_EQ(get_cmd.Files().size(), kFiles.size());
  for (std::size_t i = 0; i < kFiles.size(); ++i) {
    ASSERT_EQ(get_cmd.Files()[i], kFiles[i]);
  }
}

TEST(CmdParseTest, ParsePutCmdSingleFileReturnsSuccess) {
  std::string cmdline = "put foo";
  tftp::client::PutCmd put_cmd;

  ASSERT_EQ(put_cmd.Parse(cmdline), tftp::client::ParseStatus::kSuccess);
  ASSERT_TRUE(put_cmd.LocalFile().empty());
  ASSERT_TRUE(put_cmd.RemoteFile().empty());
  ASSERT_EQ(put_cmd.Files().size(), 1);
  ASSERT_EQ(put_cmd.Files()[0], "foo");
}

TEST(CmdParseTest, ParsePutCmdWithLocalAndRemoteFileReturnsSuccess) {
  tftp::client::File local_file = "/local/file";
  tftp::client::File remote_file = "localhost:/remote/file";
  std::string cmdline = "put " + local_file + " " + remote_file;
  tftp::client::PutCmd put_cmd;

  ASSERT_EQ(put_cmd.Parse(cmdline), tftp::client::ParseStatus::kSuccess);
  ASSERT_EQ(put_cmd.LocalFile(), local_file);
  ASSERT_EQ(put_cmd.RemoteFile(), remote_file);
  ASSERT_TRUE(put_cmd.Files().empty());
}

TEST(CmdParseTest, ParsePutCmdMultiFileAndRemoteDirReturnsSuccess) {
  std::string cmdline = "put";
  const tftp::client::FileList kFiles = {"foo", "bar", "baz", "gaz",
                                         "/remote/dir"};
  std::for_each(kFiles.cbegin(), kFiles.cend(),
                [&cmdline](const auto& f) { cmdline += " " + f; });

  tftp::client::PutCmd put_cmd;
  ASSERT_EQ(put_cmd.Parse(cmdline), tftp::client::ParseStatus::kSuccess);
  ASSERT_TRUE(put_cmd.LocalFile().empty());
  ASSERT_TRUE(put_cmd.RemoteFile().empty());

  ASSERT_EQ(put_cmd.Files().size(), kFiles.size() - 1);
  for (std::size_t i = 0; i < (kFiles.size() - 1); ++i) {
    ASSERT_EQ(put_cmd.Files()[i], kFiles[i]);
  }
  ASSERT_EQ(put_cmd.RemoteDir(), kFiles.back());
}
