#ifndef CMD_H_
#define CMD_H_

#include <cstdint>
#include <expected>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "client/config.h"
#include "common/types.h"

namespace tftp {
namespace client {

class Cmd;
enum ParseStatus : int;

using Id = std::string;
using Seconds = uint32_t;
using File = std::string;
using FileList = std::vector<File>;
using CmdPtr = std::shared_ptr<Cmd>;
template <typename T>
using ExpectedCmd = std::expected<std::shared_ptr<T>, ParseStatus>;

namespace CmdId {
constexpr Id kConnect = "connect";
constexpr Id kGet = "get";
constexpr Id kPut = "put";
constexpr Id kLiteral = "literal";
constexpr Id kMode = "mode";
constexpr Id kStatus = "status";
constexpr Id kTimeout = "timeout";
constexpr Id kRexmt = "rexmt";
constexpr Id kHelp = "?";
}  // namespace CmdId

enum ParseStatus : int {
  kSuccessfulParse = 0,
  kInvalidNumArgs,
  kInvalidPortNum,
  kInvalidMode,
  kInvalidTimeout,
  kParseStatusCnt,
};

enum ExecStatus : int {
  kSuccessfulExec = 0,
  kNotImplemented,
  kExecStatusCnt,
};

constexpr std::array<const char*, ParseStatus::kParseStatusCnt>
    kParseStatusToStr = {"success", "invalid number of arguments",
                         "invalid port number", "invalid mode",
                         "invalid timeout"};
constexpr std::array<const char*, ExecStatus::kExecStatusCnt> kExecStatusToStr =
    {"success", "command not implemented"};

class Cmd {
 public:
  virtual ExecStatus Execute([[gnu::unused]] ConfigPtr config) {
    return ExecStatus::kNotImplemented;
  }

  const Id& Id() const { return id_; }

 protected:
  Cmd() = delete;
  explicit Cmd(const tftp::client::Id& id) : id_(id) {}

  tftp::client::Id id_;
};

class GetCmd : public Cmd {
 public:
  static ExpectedCmd<GetCmd> Create(std::string_view cmdline);

  virtual ~GetCmd() = default;

  ExecStatus Execute(ConfigPtr config) final;

  const File& RemoteFile() const { return remote_file_; }
  const File& LocalFile() const { return local_file_; }
  const FileList& Files() const { return files_; }

 private:
  GetCmd() = delete;
  GetCmd(const File& remote_file, const File& local_file, const FileList& files)
      : Cmd(CmdId::kGet),
        remote_file_(remote_file),
        local_file_(local_file),
        files_(files) {}

  File remote_file_;
  File local_file_;
  FileList files_;
};

class PutCmd : public Cmd {
 public:
  static ExpectedCmd<PutCmd> Create(std::string_view cmdline);

  virtual ~PutCmd() = default;

  ExecStatus Execute(ConfigPtr config) final;

  const File& RemoteFile() const { return remote_file_; }
  const File& LocalFile() const { return local_file_; }
  const File& RemoteDir() const { return remote_dir_; }
  const FileList& Files() const { return files_; }

 private:
  PutCmd() = delete;
  PutCmd(const File& remote_file, const File& local_file,
         const File& remote_dir, const FileList& files)
      : Cmd(CmdId::kPut),
        remote_file_(remote_file),
        local_file_(local_file),
        remote_dir_(remote_dir),
        files_(files) {}

  File remote_file_;
  File local_file_;
  File remote_dir_;
  FileList files_;
};

class ConnectCmd : public Cmd {
 public:
  static ExpectedCmd<ConnectCmd> Create(std::string_view cmdline);

  virtual ~ConnectCmd() = default;

  ExecStatus Execute(ConfigPtr config) final;

  std::string Host() const { return host_; }
  uint16_t Port() const { return port_; }

 private:
  ConnectCmd() = delete;
  ConnectCmd(const HostName& host, uint16_t port)
      : Cmd(CmdId::kConnect), host_(host), port_(port) {}

  std::string host_;
  uint16_t port_ = 0;
};

class LiteralCmd : public Cmd {
 public:
  static ExpectedCmd<LiteralCmd> Create();

  virtual ~LiteralCmd() = default;

  ExecStatus Execute(ConfigPtr config) final;

  bool LiteralMode() const { return literal_mode_; }

 private:
  LiteralCmd() : Cmd(CmdId::kLiteral), literal_mode_(false) {}

  bool literal_mode_;
};

class ModeCmd : public Cmd {
 public:
  static ExpectedCmd<ModeCmd> Create(std::string_view cmdline);

  virtual ~ModeCmd() = default;

  ExecStatus Execute(ConfigPtr config) final;

  const Mode& Mode() const { return mode_; }

 private:
  ModeCmd() = delete;
  explicit ModeCmd(const tftp::Mode mode) : Cmd(CmdId::kMode), mode_(mode) {}

  tftp::Mode mode_;
};

class StatusCmd : public Cmd {
 public:
  static ExpectedCmd<StatusCmd> Create();

  virtual ~StatusCmd() = default;

  ExecStatus Execute(ConfigPtr config) final;

 private:
  StatusCmd() : Cmd(CmdId::kStatus) {}
};

class TimeoutCmd : public Cmd {
 public:
  static ExpectedCmd<TimeoutCmd> Create(std::string_view cmdline);

  virtual ~TimeoutCmd() = default;

  ExecStatus Execute(ConfigPtr config) final;

  Seconds Timeout() const { return timeout_; }

 private:
  TimeoutCmd() = delete;
  explicit TimeoutCmd(Seconds timeout)
      : Cmd(CmdId::kTimeout), timeout_(timeout) {}

  Seconds timeout_;
};

class RexmtCmd : public Cmd {
 public:
  static ExpectedCmd<RexmtCmd> Create(std::string_view cmdline);

  virtual ~RexmtCmd() = default;

  ExecStatus Execute(ConfigPtr config) final;

  Seconds RexmtTimeout() const { return rexmt_timeout_; }

 private:
  RexmtCmd() = delete;
  explicit RexmtCmd(Seconds rexmt_timeout)
      : Cmd(CmdId::kRexmt), rexmt_timeout_(rexmt_timeout) {}

  Seconds rexmt_timeout_;
};

class HelpCmd : public Cmd {
 public:
  static ExpectedCmd<HelpCmd> Create(std::string_view cmdline);

  virtual ~HelpCmd() = default;

  ExecStatus Execute(ConfigPtr config) final;

  const tftp::client::Id& TargetCmd() const { return target_cmd_; }

 private:
  HelpCmd() = delete;
  explicit HelpCmd(const tftp::client::Id& target_cmd)
      : Cmd(CmdId::kHelp), target_cmd_(target_cmd) {}

  tftp::client::Id target_cmd_;
};

}  // namespace client
}  // namespace tftp

#endif
