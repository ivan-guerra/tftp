#ifndef CMD_H_
#define CMD_H_

#include <cstdint>
#include <expected>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <vector>

#include "client/config.h"
#include "common/types.h"

namespace tftp {
namespace client {

class Cmd;
struct CmdStatus;
struct ClientState;
enum ParseStatus : int;
enum ExecStatus : int;

using Id = std::string;
using Seconds = uint32_t;
using File = std::string;
using FileList = std::vector<File>;
using CmdPtr = std::shared_ptr<Cmd>;
using ClientStatePtr = std::shared_ptr<ClientState>;
using ActiveCmds = std::map<int, CmdPtr>;
using CompleteCmds = std::map<int, ExecStatus>;

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
constexpr Id kQuit = "quit";
constexpr Id kHelp = "?";
}  // namespace CmdId

enum ParseStatus : int {
  kSuccess = 0,
  kInvalidNumArgs,
  kInvalidPortNum,
  kInvalidMode,
  kInvalidTimeout,
  kStatusCount,
};

enum ExecStatus : int {
  kNotImplemented,
};

struct ClientState {
  std::mutex state_mtx;

  int cmd_total = 0;
  ConfigPtr conf = nullptr;
  ActiveCmds running;
  CompleteCmds done;
};

constexpr std::array<const char*, ParseStatus::kStatusCount> kParseStatusToStr =
    {"success", "invalid number of arguments", "invalid port number",
     "invalid mode", "invalid timeout"};

class Cmd {
 public:
  virtual ExecStatus Execute([[gnu::unused]] ClientStatePtr config) {
    return ExecStatus::kNotImplemented;
  }

  const Id& Id() const { return id_; }
  double PercentComplete() const { return percent_complete_; }

 protected:
  Cmd() = delete;
  explicit Cmd(const tftp::client::Id& id) : id_(id), percent_complete_(0.0) {}

  tftp::client::Id id_;
  double percent_complete_;
};

class GetCmd : public Cmd {
 public:
  static ExpectedCmd<GetCmd> Create(std::string_view cmdline);

  GetCmd() = delete;
  GetCmd(const File& remote_file, const File& local_file, const FileList& files)
      : Cmd(CmdId::kGet),
        remote_file_(remote_file),
        local_file_(local_file),
        files_(files) {}

  virtual ~GetCmd() = default;

  ExecStatus Execute(ClientStatePtr config) final;

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
  static ExpectedCmd<PutCmd> Create(std::string_view cmdline);

  PutCmd() = delete;
  PutCmd(const File& remote_file, const File& local_file,
         const File& remote_dir, const FileList& files)
      : Cmd(CmdId::kPut),
        remote_file_(remote_file),
        local_file_(local_file),
        remote_dir_(remote_dir),
        files_(files) {}

  virtual ~PutCmd() = default;

  ExecStatus Execute(ClientStatePtr config) final;

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
  static ExpectedCmd<ConnectCmd> Create(std::string_view cmdline);

  ConnectCmd() = delete;
  ConnectCmd(const HostName& host, uint16_t port)
      : Cmd(CmdId::kConnect), host_(host), port_(port) {}

  virtual ~ConnectCmd() = default;

  ExecStatus Execute(ClientStatePtr config) final;

  std::string Host() const { return host_; }
  uint16_t Port() const { return port_; }

 private:
  std::string host_;
  uint16_t port_ = 0;
};

class LiteralCmd : public Cmd {
 public:
  static ExpectedCmd<LiteralCmd> Create();

  LiteralCmd() : Cmd(CmdId::kLiteral), literal_mode_(false) {}

  virtual ~LiteralCmd() = default;

  ExecStatus Execute(ClientStatePtr config) final;

  bool LiteralMode() const { return literal_mode_; }

 private:
  bool literal_mode_;
};

class ModeCmd : public Cmd {
 public:
  static ExpectedCmd<ModeCmd> Create(std::string_view cmdline);

  ModeCmd() = delete;
  explicit ModeCmd(const tftp::Mode mode) : Cmd(CmdId::kMode), mode_(mode) {}

  virtual ~ModeCmd() = default;

  ExecStatus Execute(ClientStatePtr config) final;

  const Mode& Mode() const { return mode_; }

 private:
  tftp::Mode mode_;
};

class StatusCmd : public Cmd {
 public:
  static ExpectedCmd<StatusCmd> Create();

  StatusCmd() : Cmd(CmdId::kStatus) {}

  virtual ~StatusCmd() = default;

  ExecStatus Execute(ClientStatePtr config) final;

 private:
};

class TimeoutCmd : public Cmd {
 public:
  static ExpectedCmd<TimeoutCmd> Create(std::string_view cmdline);

  TimeoutCmd() = delete;
  explicit TimeoutCmd(Seconds timeout)
      : Cmd(CmdId::kTimeout), timeout_(timeout) {}

  virtual ~TimeoutCmd() = default;

  ExecStatus Execute(ClientStatePtr config) final;

  Seconds Timeout() const { return timeout_; }

 private:
  Seconds timeout_;
};

class RexmtCmd : public Cmd {
 public:
  static ExpectedCmd<RexmtCmd> Create(std::string_view cmdline);

  RexmtCmd() = delete;
  explicit RexmtCmd(Seconds rexmt_timeout)
      : Cmd(CmdId::kRexmt), rexmt_timeout_(rexmt_timeout) {}

  virtual ~RexmtCmd() = default;

  ExecStatus Execute(ClientStatePtr config) final;

  Seconds RexmtTimeout() const { return rexmt_timeout_; }

 private:
  Seconds rexmt_timeout_;
};

class QuitCmd : public Cmd {
 public:
  static ExpectedCmd<QuitCmd> Create();

  QuitCmd() : Cmd(CmdId::kQuit) {}

  virtual ~QuitCmd() = default;

  ExecStatus Execute(ClientStatePtr config) final;

 private:
};

class HelpCmd : public Cmd {
 public:
  static ExpectedCmd<HelpCmd> Create(std::string_view cmdline);

  HelpCmd() = delete;
  explicit HelpCmd(const tftp::client::Id& target_cmd)
      : Cmd(CmdId::kHelp), target_cmd_(target_cmd) {}

  virtual ~HelpCmd() = default;

  ExecStatus Execute(ClientStatePtr config) final;

  const tftp::client::Id& TargetCmd() const { return target_cmd_; }

 private:
  tftp::client::Id target_cmd_;
};

}  // namespace client
}  // namespace tftp

#endif
