#include "client/cmd.h"

#include <cstdint>
#include <expected>
#include <iostream>
#include <iterator>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include "common/parse.h"
#include "common/types.h"

namespace tftp {
namespace client {

using Token = std::string;
using TokenList = std::vector<std::string>;

static TokenList Tokenize(std::string_view cmdline) {
  std::istringstream buffer(cmdline.data());

  return {std::istream_iterator<Token>(buffer), {}};
}

ExecStatus ConnectCmd::Execute([[gnu::unused]] ConfigPtr config) {
  return ExecStatus::kNotImplemented;
}

ExpectedCmd<ConnectCmd> ConnectCmd::Create(std::string_view cmdline) {
  TokenList args = Tokenize(cmdline);
  if (args.size() < 2 || args.size() > 3) {
    return std::unexpected(ParseStatus::kInvalidNumArgs);
  }

  std::expected<uint16_t, ParseStatus> port = 0;
  if (args.size() == 3) {
    port = ParsePort(args[2]);
    if (!port) {
      return std::unexpected(port.error());
    }
  }

  return std::shared_ptr<ConnectCmd>(new ConnectCmd(args[1], *port));
}

ExecStatus GetCmd::Execute([[gnu::unused]] ConfigPtr config) {
  return ExecStatus::kNotImplemented;
}

ExpectedCmd<GetCmd> GetCmd::Create(std::string_view cmdline) {
  TokenList args = Tokenize(cmdline);
  if (args.size() <= 1) {
    return std::unexpected(ParseStatus::kInvalidNumArgs);
  }
  args.erase(args.begin()); /* Erase the command code. */

  FileList files;
  if (args.size() == 1) { /* Fetch a single file. */
    files = {args[0]};
  }

  File remote_file;
  File local_file;
  if (args.size() == 2) { /* Fetch remotefile to localfile */
    remote_file = args[0];
    local_file = args[1];
  }

  if (args.size() > 2) { /* Fetching a set of files. */
    files = args;
  }

  return std::shared_ptr<GetCmd>(new GetCmd(remote_file, local_file, files));
}

ExecStatus PutCmd::Execute([[gnu::unused]] ConfigPtr config) {
  return ExecStatus::kNotImplemented;
}

ExpectedCmd<PutCmd> PutCmd::Create(std::string_view cmdline) {
  TokenList args = Tokenize(cmdline);
  if (args.size() <= 1) {
    return std::unexpected(ParseStatus::kInvalidNumArgs);
  }
  args.erase(args.begin()); /* Erase the command code. */

  FileList files;
  if (args.size() == 1) { /* Transfer a single file. */
    files = {args[0]};
  }

  File local_file;
  File remote_file;
  if (args.size() == 2) { /* Transfer remotefile to localfile */
    local_file = args[0];
    remote_file = args[1];
  }

  File remote_dir;
  if (args.size() > 2) { /* Transferring a set of files to a remote dir. */
    remote_dir = args.back();
    files = FileList(args.cbegin(), args.cbegin() + args.size() - 1);
  }

  return std::shared_ptr<PutCmd>(
      new PutCmd(remote_file, local_file, remote_dir, files));
}

ExecStatus LiteralCmd::Execute(ConfigPtr config) {
  config->SetLiteralMode(!config->GetLiteralMode());

  return ExecStatus::kSuccessfulExec;
}

ExpectedCmd<LiteralCmd> LiteralCmd::Create() {
  return std::shared_ptr<LiteralCmd>(new LiteralCmd());
}

ExecStatus ModeCmd::Execute(ConfigPtr config) {
  config->SetMode(mode_);
  return ExecStatus::kSuccessfulExec;
}

ExpectedCmd<ModeCmd> ModeCmd::Create(std::string_view cmdline) {
  TokenList args = Tokenize(cmdline);
  if (args.size() != 2) {
    return std::unexpected(ParseStatus::kInvalidNumArgs);
  }

  auto mode = ParseMode(args[1]);
  if (!mode) {
    return std::unexpected(mode.error());
  }

  return std::shared_ptr<ModeCmd>(new ModeCmd(*mode));
}

ExecStatus StatusCmd::Execute(ConfigPtr config) {
  std::cout << "\tmode: " << config->GetMode() << std::endl;
  std::cout << "\tliteral mode enabled: " << std::boolalpha
            << config->GetLiteralMode() << std::endl;
  std::cout << "\thostname: " << config->GetHostname() << std::endl;
  std::cout << "\tports: " << config->GetPortRange().start << "-"
            << config->GetPortRange().end << std::endl;
  std::cout << "\ttransmission timeout (sec): " << config->GetTimeout()
            << std::endl;
  std::cout << "\trexmt timeout (sec): " << config->GetRexmtTimeout()
            << std::endl;

  return ExecStatus::kSuccessfulExec;
}

ExpectedCmd<StatusCmd> StatusCmd::Create() {
  return std::shared_ptr<StatusCmd>(new StatusCmd());
}

ExecStatus TimeoutCmd::Execute([[gnu::unused]] ConfigPtr config) {
  return ExecStatus::kNotImplemented;
}

ExpectedCmd<TimeoutCmd> TimeoutCmd::Create(std::string_view cmdline) {
  TokenList args = Tokenize(cmdline);
  if (args.size() != 2) {
    return std::unexpected(ParseStatus::kInvalidNumArgs);
  }

  auto timeout = ParseTimeValue(args[1]);
  if (!timeout) {
    return std::unexpected(timeout.error());
  }

  return std::shared_ptr<TimeoutCmd>(new TimeoutCmd(*timeout));
}

ExecStatus RexmtCmd::Execute([[gnu::unused]] ConfigPtr config) {
  return ExecStatus::kNotImplemented;
}

ExpectedCmd<RexmtCmd> RexmtCmd::Create(std::string_view cmdline) {
  TokenList args = Tokenize(cmdline);
  if (args.size() != 2) {
    return std::unexpected(ParseStatus::kInvalidNumArgs);
  }

  auto rexmt_timeout = ParseTimeValue(args[1]);
  if (!rexmt_timeout) {
    return std::unexpected(rexmt_timeout.error());
  }

  return std::shared_ptr<RexmtCmd>(new RexmtCmd(*rexmt_timeout));
}

ExecStatus HelpCmd::Execute([[gnu::unused]] ConfigPtr config) {
  return ExecStatus::kNotImplemented;
}

ExpectedCmd<HelpCmd> HelpCmd::Create(std::string_view cmdline) {
  TokenList args = Tokenize(cmdline);
  if (args.size() != 2) {
    return std::unexpected(ParseStatus::kInvalidNumArgs);
  }

  return std::shared_ptr<HelpCmd>(new HelpCmd(args[1]));
}

}  // namespace client
}  // namespace tftp
