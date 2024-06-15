#ifndef CMD_H_
#define CMD_H_

#include <array>
#include <cstdint>
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
};

constexpr std::array<const char*, ParseStatus::kStatusCount> kParseStatusToStr =
    {"success", "invalid number of arguments", "invalid port number",
     "invalid mode", "invalid timeout"};

class Cmd {
 public:
  virtual ExecStatus Execute(client::Config& config) = 0;
  virtual ParseStatus Parse(std::string_view cmdline) = 0;

  const Id& Id() const { return id_; }

 protected:
  Cmd() = delete;
  explicit Cmd(const tftp::client::Id& id) : id_(id) {}

  tftp::client::Id id_;
};

class GetCmd : public Cmd {
 public:
  GetCmd() : Cmd(CmdId::kGet) {}
  virtual ~GetCmd() = default;

  ExecStatus Execute(client::Config& config) final;
  ParseStatus Parse(std::string_view cmdline) final;

  const File& RemoteFile() const { return remote_file_; }
  const File& LocalFile() const { return local_file_; }
  const FileList& Files() const { return files_; }

 private:
  File remote_file_;
  File local_file_;
  FileList files_;
};

class PutCmd : public Cmd {
 public:
  PutCmd() : Cmd(CmdId::kPut) {}
  virtual ~PutCmd() = default;

  ExecStatus Execute(client::Config& config) final;
  ParseStatus Parse(std::string_view cmdline) final;

  const File& RemoteFile() const { return remote_file_; }
  const File& LocalFile() const { return local_file_; }
  const File& RemoteDir() const { return remote_dir_; }
  const FileList& Files() const { return files_; }

 private:
  File remote_file_;
  File local_file_;
  File remote_dir_;
  FileList files_;
};

class ConnectCmd : public Cmd {
 public:
  ConnectCmd() : Cmd(CmdId::kConnect), host_(""), port_(0) {}
  virtual ~ConnectCmd() = default;

  ExecStatus Execute(client::Config& config) final;
  ParseStatus Parse(std::string_view cmdline) final;

  std::string Host() const { return host_; }
  uint16_t Port() const { return port_; }

 private:
  std::string host_;
  uint16_t port_ = 0;
};

class LiteralCmd : public Cmd {
 public:
  LiteralCmd() : Cmd(CmdId::kLiteral), literal_mode_(false) {}
  virtual ~LiteralCmd() = default;

  ExecStatus Execute(client::Config& config) final;
  ParseStatus Parse(std::string_view cmdline) final;

  bool LiteralMode() const { return literal_mode_; }

 private:
  bool literal_mode_;
};

class ModeCmd : public Cmd {
 public:
  ModeCmd() : Cmd(CmdId::kMode) {}
  virtual ~ModeCmd() = default;

  ExecStatus Execute(client::Config& config) final;
  ParseStatus Parse(std::string_view cmdline) final;

  const Mode& Mode() const { return mode_; }

 private:
  tftp::Mode mode_;
};

class StatusCmd : public Cmd {
 public:
  StatusCmd() : Cmd(CmdId::kStatus) {}
  virtual ~StatusCmd() = default;

  ExecStatus Execute(client::Config& config) final;
  ParseStatus Parse(std::string_view cmdline) final;
};

class TimeoutCmd : public Cmd {
 public:
  TimeoutCmd() : Cmd(CmdId::kTimeout), timeout_(0) {}
  virtual ~TimeoutCmd() = default;

  ExecStatus Execute(client::Config& config) final;
  ParseStatus Parse(std::string_view cmdline) final;

  Seconds Timeout() const { return timeout_; }

 private:
  Seconds timeout_;
};

class RexmtCmd : public Cmd {
 public:
  RexmtCmd() : Cmd(CmdId::kRexmt), rextm_timeout_(0) {}
  virtual ~RexmtCmd() = default;

  ExecStatus Execute(client::Config& config) final;
  ParseStatus Parse(std::string_view cmdline) final;

  Seconds RexmtTimeout() const { return rextm_timeout_; }

 private:
  Seconds rextm_timeout_;
};

class QuitCmd : public Cmd {
 public:
  QuitCmd() : Cmd(CmdId::kQuit) {}
  virtual ~QuitCmd() = default;

  ExecStatus Execute(client::Config& config) final;
  ParseStatus Parse(std::string_view cmdline) final;
};

class HelpCmd : public Cmd {
 public:
  HelpCmd() : Cmd(CmdId::kHelp) {}
  virtual ~HelpCmd() = default;

  ExecStatus Execute(client::Config& config) final;
  ParseStatus Parse(std::string_view cmdline) final;

  const tftp::client::Id& TargetCmd() const { return target_cmd_; }

 private:
  tftp::client::Id target_cmd_;
};

}  // namespace client
}  // namespace tftp

#endif
