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

ExecStatus ConnectCmd::Execute(Config& conf) {
  conf.hostname = host_;
  conf.ports.start = port_;
  conf.ports.end = port_;

  return ExecStatus::kSuccessfulExec;
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

void ConnectCmd::PrintUsage() {
  std::cout << "connect host [port]" << std::endl;
  std::cout << "    Set the host (and optionally port) for transfers. Note "
               "that the TFTP"
            << std::endl;
  std::cout << "    protocol, unlike the FTP protocol, does not maintain "
               "connections between"
            << std::endl;
  std::cout << "    transfers; thus, the connect command does not actually "
               "create a connection,"
            << std::endl;
  std::cout << "    but merely remembers what host is to be used for "
               "transfers. You do not have"
            << std::endl;
  std::cout << "    to use the connect command; the remote host can be "
               "specified as part of the"
            << std::endl;
  std::cout << "    get or put commands." << std::endl;
}

ExecStatus GetCmd::Execute([[gnu::unused]] Config& conf) {
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

void GetCmd::PrintUsage() {
  std::cout << "get file" << std::endl;
  std::cout << "get remotefile localfile" << std::endl;
  std::cout << "get file1 file2 file3..." << std::endl;
  std::cout << "    Get a file or set of files from the specified sources. A "
               "remote filename can"
            << std::endl;
  std::cout << "    be in one of two forms: a plain filename on the remote "
               "host, if the host has"
            << std::endl;
  std::cout << "    already been specified, or a string of the form "
               "host:filename to specify"
            << std::endl;
  std::cout << "    both a host and filename at the same time. If the latter "
               "form is used, the"
            << std::endl;
  std::cout << "    last hostname specified becomes the default for future "
               "transfers. Enable"
            << std::endl;
  std::cout << "    literal mode to prevent special treatment of the ':' "
               "character (e.g."
            << std::endl;
  std::cout << "    C:\\dir\\file)." << std::endl;
}

ExecStatus PutCmd::Execute([[gnu::unused]] Config& conf) {
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

void PutCmd::PrintUsage() {
  std::cout << "put file" << std::endl;
  std::cout << "put localfile remotefile" << std::endl;
  std::cout << "put file1 file2 file3... remote-directory" << std::endl;
  std::cout << "    Put a file or set of files to the specified remote file or "
               "directory. The"
            << std::endl;
  std::cout << "    destination can be in one of two forms: a filename on the "
               "remote host, if"
            << std::endl;
  std::cout << "    the host has already been specified, or a string of the "
               "form host:filename"
            << std::endl;
  std::cout << "    to specify both a host and filename at the same time. If "
               "the latter form is"
            << std::endl;
  std::cout << "    used, the hostname specified becomes the default for "
               "future transfers. If"
            << std::endl;
  std::cout << "    the remote-directory form is used, the remote host is "
               "assumed to be a UNIX"
            << std::endl;
  std::cout << "    system or another system using / as directory separator. "
               "Enable literal mode"
            << std::endl;
  std::cout << "    to prevent special treatment of the ':' character (e.g. "
               "C:\\dir\\file)."
            << std::endl;
}

ExecStatus LiteralCmd::Execute(Config& conf) {
  conf.literal_mode = !conf.literal_mode;

  return ExecStatus::kSuccessfulExec;
}

ExpectedCmd<LiteralCmd> LiteralCmd::Create() {
  return std::shared_ptr<LiteralCmd>(new LiteralCmd());
}

void LiteralCmd::PrintUsage() {
  std::cout << "literal" << std::endl;
  std::cout << "    Toggle literal mode. When set, this mode prevents special "
               "treatment of ':'"
            << std::endl;
  std::cout << "    in filenames." << std::endl;
}

ExecStatus ModeCmd::Execute(Config& conf) {
  conf.mode = mode_;
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

void ModeCmd::PrintUsage() {
  std::cout << "mode transfer-mode" << std::endl;
  std::cout
      << "    Specify the mode for transfers; transfer-mode may be one of "
         "ascii (or"
      << std::endl;
  std::cout << "    netascii) or binary (or octet.) The default is ascii. "
            << std::endl;
}

ExecStatus StatusCmd::Execute(Config& conf) {
  std::cout << "\tmode: " << conf.mode << std::endl;
  std::cout << "\tliteral mode enabled: " << std::boolalpha << conf.literal_mode
            << std::endl;
  std::cout << "\thostname: " << conf.hostname << std::endl;
  if (conf.ports.start == conf.ports.end) {
    std::cout << "\tport: " << conf.ports.start << std::endl;
  } else {
    std::cout << "\tports: " << conf.ports.start << "-" << conf.ports.end
              << std::endl;
  }
  std::cout << "\ttransmission timeout (sec): " << conf.timeout << std::endl;
  std::cout << "\trexmt timeout (sec): " << conf.rexmt_timeout << std::endl;

  return ExecStatus::kSuccessfulExec;
}

ExpectedCmd<StatusCmd> StatusCmd::Create() {
  return std::shared_ptr<StatusCmd>(new StatusCmd());
}

void StatusCmd::PrintUsage() {
  std::cout << "status" << std::endl;
  std::cout << "    Show current status." << std::endl;
}

ExecStatus TimeoutCmd::Execute(Config& conf) {
  conf.timeout = timeout_;

  return ExecStatus::kSuccessfulExec;
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

void TimeoutCmd::PrintUsage() {
  std::cout << "timeout total-transmission-timeout" << std::endl;
  std::cout << "    Set the total transmission timeout, in seconds."
            << std::endl;
}

ExecStatus RexmtCmd::Execute(Config& conf) {
  conf.rexmt_timeout = rexmt_timeout_;

  return ExecStatus::kSuccessfulExec;
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

void RexmtCmd::PrintUsage() {
  std::cout << "rexmt retransmission-timeout" << std::endl;
  std::cout << "    Set the per-packet retransmission timeout, in seconds."
            << std::endl;
}

ExecStatus HelpCmd::Execute([[gnu::unused]] Config& conf) {
  if (CmdId::kGet == target_cmd_) {
    GetCmd::PrintUsage();
  } else if (CmdId::kPut == target_cmd_) {
    PutCmd::PrintUsage();
  } else if (CmdId::kMode == target_cmd_) {
    ModeCmd::PrintUsage();
  } else if (CmdId::kRexmt == target_cmd_) {
    RexmtCmd::PrintUsage();
  } else if (CmdId::kStatus == target_cmd_) {
    StatusCmd::PrintUsage();
  } else if (CmdId::kConnect == target_cmd_) {
    ConnectCmd::PrintUsage();
  } else if (CmdId::kLiteral == target_cmd_) {
    LiteralCmd::PrintUsage();
  } else if (CmdId::kTimeout == target_cmd_) {
    TimeoutCmd::PrintUsage();
  } else if (CmdId::kHelp == target_cmd_) {
    HelpCmd::PrintUsage();
  } else {
    return ExecStatus::kUnknownCmdHelp;
  }
  return ExecStatus::kSuccessfulExec;
}

ExpectedCmd<HelpCmd> HelpCmd::Create(std::string_view cmdline) {
  TokenList args = Tokenize(cmdline);
  if (args.size() != 2) {
    return std::unexpected(ParseStatus::kInvalidNumArgs);
  }

  return std::shared_ptr<HelpCmd>(new HelpCmd(args[1]));
}

void HelpCmd::PrintUsage() {
  std::cout << "help command-name" << std::endl;
  std::cout << "    Print help information" << std::endl;
}

}  // namespace client
}  // namespace tftp
