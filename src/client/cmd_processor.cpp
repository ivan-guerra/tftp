#include "client/cmd_processor.h"

#include <expected>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>

#include "client/cmd.h"

namespace tftp {
namespace client {

void CmdProcessor::PrintError(std::string_view err_msg) const {
  std::cout << "error: " << err_msg << std::endl;
}

std::expected<CmdPtr, ParseStatus> CmdProcessor::LoadCmd(
    std::string_view cmdline) const {
  /* Parse the command ID from the command line. */
  std::istringstream is(cmdline.data());
  Id cmd_id;
  is >> cmd_id;

  /* Create the command object. */
  std::expected<CmdPtr, ParseStatus> cmd;
  if (cmd_id == CmdId::kGet) {
    cmd = CreateCmd<GetCmd>(cmdline);
  } else if (cmd_id == CmdId::kPut) {
    cmd = CreateCmd<PutCmd>(cmdline);
  } else if (cmd_id == CmdId::kHelp) {
    cmd = CreateCmd<HelpCmd>(cmdline);
  } else if (cmd_id == CmdId::kMode) {
    cmd = CreateCmd<ModeCmd>(cmdline);
  } else if (cmd_id == CmdId::kStatus) {
    cmd = CreateCmd<StatusCmd>();
  } else if (cmd_id == CmdId::kConnect) {
    cmd = CreateCmd<ConnectCmd>(cmdline);
  } else if (cmd_id == CmdId::kLiteral) {
    cmd = CreateCmd<LiteralCmd>();
  } else if (cmd_id == CmdId::kTimeout) {
    cmd = CreateCmd<TimeoutCmd>(cmdline);
  } else if (cmd_id == CmdId::kRexmt) {
    cmd = CreateCmd<RexmtCmd>(cmdline);
  } else {
    return std::unexpected(ParseStatus::kUnknownCmd);
  }
  return cmd;
}

void CmdProcessor::Run() {
  static const char *kPrompt = "tftp> ";
  std::cout << kPrompt;

  std::string line;
  while (std::getline(std::cin, line)) {
    if (line == "quit") {
      break;
    }

    if (!line.empty()) {
      auto cmd = LoadCmd(line);
      if (!cmd) { /* Unable to parse the commandline successfully. */
        PrintError(kParseStatusToStr[cmd.error()]);
      } else { /* Got a valid command, execute it. */
        ExecStatus exec_stat = (*cmd)->Execute(conf_);
        if (exec_stat != ExecStatus::kSuccessfulExec) {
          PrintError(kExecStatusToStr[exec_stat]);
        }
      }
    }
    std::cout << kPrompt;
  }
}

}  // namespace client
}  // namespace tftp
