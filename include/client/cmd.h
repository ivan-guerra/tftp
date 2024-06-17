#ifndef CMD_H_
#define CMD_H_

#include <array>
#include <cstdint>
#include <expected>
#include <string>
#include <string_view>
#include <vector>

#include "client/config.h"
#include "common/types.h"

namespace tftp {
namespace client {

using Id = std::string;
using Seconds = uint32_t;
using File = std::string;
using FileList = std::vector<File>;

namespace CmdId {
constexpr Id kConnect = "connect";
constexpr Id kGet = "get";
constexpr Id kPut = "put";
constexpr Id kLiteral = "literal";
constexpr Id kMode = "mode";
constexpr Id kStatus = "status";
constexpr Id kTimeout = "timeout";
constexpr Id kRexmt = "rexmt";
constexpr Id kQuit = "quit";
constexpr Id kHelp = "?";
}  // namespace CmdId

enum ParseStatus {
  kSuccess = 0,
  kInvalidNumArgs,
  kInvalidPortNum,
  kInvalidMode,
  kInvalidTimeout,
  kStatusCount,
};

enum class ExecStatus {
  kSuccess,
  kNotImplemented,
};

constexpr std::array<const char*, ParseStatus::kStatusCount> kParseStatusToStr =
    {"success", "invalid number of arguments", "invalid port number",
     "invalid mode", "invalid timeout"};

class Cmd {
 public:
  virtual ExecStatus Execute([[gnu::unused]] client::Config& config) {
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
  static std::expected<GetCmd, ParseStatus> Create(std::string_view cmdline);

  virtual ~GetCmd() = default;

  ExecStatus Execute(client::Config& config) final;

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
  static std::expected<PutCmd, ParseStatus> Create(std::string_view cmdline);

  virtual ~PutCmd() = default;

  ExecStatus Execute(client::Config& config) final;

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
  static std::expected<ConnectCmd, ParseStatus> Create(
      std::string_view cmdline);

  virtual ~ConnectCmd() = default;

  ExecStatus Execute(client::Config& config) final;

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
  static std::expected<LiteralCmd, ParseStatus> Create();

  virtual ~LiteralCmd() = default;

  ExecStatus Execute(client::Config& config) final;

  bool LiteralMode() const { return literal_mode_; }

 private:
  LiteralCmd() : Cmd(CmdId::kLiteral), literal_mode_(false) {}

  bool literal_mode_;
};

class ModeCmd : public Cmd {
 public:
  static std::expected<ModeCmd, ParseStatus> Create(std::string_view cmdline);

  virtual ~ModeCmd() = default;

  ExecStatus Execute(client::Config& config) final;

  const Mode& Mode() const { return mode_; }

 private:
  ModeCmd() = delete;
  explicit ModeCmd(const tftp::Mode mode) : Cmd(CmdId::kMode), mode_(mode) {}

  tftp::Mode mode_;
};

class StatusCmd : public Cmd {
 public:
  static std::expected<StatusCmd, ParseStatus> Create();

  virtual ~StatusCmd() = default;

  ExecStatus Execute(client::Config& config) final;

 private:
  StatusCmd() : Cmd(CmdId::kStatus) {}
};

class TimeoutCmd : public Cmd {
 public:
  static std::expected<TimeoutCmd, ParseStatus> Create(
      std::string_view cmdline);

  virtual ~TimeoutCmd() = default;

  ExecStatus Execute(client::Config& config) final;

  Seconds Timeout() const { return timeout_; }

 private:
  TimeoutCmd() = delete;
  explicit TimeoutCmd(Seconds timeout)
      : Cmd(CmdId::kTimeout), timeout_(timeout) {}

  Seconds timeout_;
};

class RexmtCmd : public Cmd {
 public:
  static std::expected<RexmtCmd, ParseStatus> Create(std::string_view cmdline);

  virtual ~RexmtCmd() = default;

  ExecStatus Execute(client::Config& config) final;

  Seconds RexmtTimeout() const { return rexmt_timeout_; }

 private:
  RexmtCmd() = delete;
  explicit RexmtCmd(Seconds rexmt_timeout)
      : Cmd(CmdId::kRexmt), rexmt_timeout_(rexmt_timeout) {}

  Seconds rexmt_timeout_;
};

class QuitCmd : public Cmd {
 public:
  static std::expected<QuitCmd, ParseStatus> Create();

  virtual ~QuitCmd() = default;

  ExecStatus Execute(client::Config& config) final;

 private:
  QuitCmd() : Cmd(CmdId::kQuit) {}
};

class HelpCmd : public Cmd {
 public:
  static std::expected<HelpCmd, ParseStatus> Create(std::string_view cmdline);

  virtual ~HelpCmd() = default;

  ExecStatus Execute(client::Config& config) final;

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
