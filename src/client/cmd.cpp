#include "client/cmd.h"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <expected>
#include <iterator>
#include <limits>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include "common/types.h"

namespace tftp {
namespace client {

using Token = std::string;
using TokenList = std::vector<std::string>;

static TokenList Tokenize(std::string_view cmdline) {
  std::istringstream buffer(cmdline.data());

  return {std::istream_iterator<Token>(buffer), {}};
}

static bool IsPositiveNum(std::string_view val) {
  return std::all_of(val.cbegin(), val.cend(),
                     [](char c) { return std::isdigit(c); });
}

static std::expected<Seconds, ParseStatus> ParseTimeValue(
    std::string_view num_str) {
  if (!IsPositiveNum(num_str)) {
    return std::unexpected(ParseStatus::kInvalidTimeout);
  }

  uint64_t timeout_tmp = std::stoull(num_str.data());
  if (timeout_tmp > std::numeric_limits<uint32_t>::max()) {
    return std::unexpected(ParseStatus::kInvalidTimeout);
  }

  return timeout_tmp;
}

ExecStatus ConnectCmd::Execute([[gnu::unused]] ClientState& config) {
  return ExecStatus::kNotImplemented;
}

std::expected<ConnectCmd, ParseStatus> ConnectCmd::Create(
    std::string_view cmdline) {
  TokenList args = Tokenize(cmdline);
  if (args.size() < 2 || args.size() > 3) {
    return std::unexpected(ParseStatus::kInvalidNumArgs);
  }

  uint16_t port = 0;
  if (args.size() == 3) {
    if (!IsPositiveNum(args[2])) {
      return std::unexpected(ParseStatus::kInvalidPortNum);
    }

    uint64_t port_tmp = std::stoull(args[2]);
    if (port_tmp > std::numeric_limits<uint16_t>::max()) {
      return std::unexpected(ParseStatus::kInvalidPortNum);
    }

    port = static_cast<uint16_t>(port_tmp);
  }

  return ConnectCmd(args[1], port);
}

ExecStatus GetCmd::Execute([[gnu::unused]] ClientState& config) {
  return ExecStatus::kNotImplemented;
}

std::expected<GetCmd, ParseStatus> GetCmd::Create(std::string_view cmdline) {
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

  return GetCmd(remote_file, local_file, files);
}

ExecStatus PutCmd::Execute([[gnu::unused]] ClientState& config) {
  return ExecStatus::kNotImplemented;
}

std::expected<PutCmd, ParseStatus> PutCmd::Create(std::string_view cmdline) {
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

  return PutCmd(remote_file, local_file, remote_dir, files);
}

ExecStatus LiteralCmd::Execute([[gnu::unused]] ClientState& config) {
  return ExecStatus::kNotImplemented;
}

std::expected<LiteralCmd, ParseStatus> LiteralCmd::Create() {
  return LiteralCmd();
}

ExecStatus ModeCmd::Execute([[gnu::unused]] ClientState& config) {
  return ExecStatus::kNotImplemented;
}

std::expected<ModeCmd, ParseStatus> ModeCmd::Create(std::string_view cmdline) {
  TokenList args = Tokenize(cmdline);
  if (args.size() != 2) {
    return std::unexpected(ParseStatus::kInvalidNumArgs);
  }

  std::string mode_lower(args[1].size(), 0);
  std::transform(args[1].cbegin(), args[1].cend(), mode_lower.begin(),
                 [](char c) { return std::tolower(c); });

  if ((mode_lower != SendMode::kNetAscii) && (mode_lower != SendMode::kOctet)) {
    return std::unexpected(ParseStatus::kInvalidMode);
  }

  return ModeCmd(mode_lower);
}

ExecStatus StatusCmd::Execute([[gnu::unused]] ClientState& config) {
  return ExecStatus::kNotImplemented;
}

std::expected<StatusCmd, ParseStatus> StatusCmd::Create() {
  return StatusCmd();
}

ExecStatus TimeoutCmd::Execute([[gnu::unused]] ClientState& config) {
  return ExecStatus::kNotImplemented;
}

std::expected<TimeoutCmd, ParseStatus> TimeoutCmd::Create(
    std::string_view cmdline) {
  TokenList args = Tokenize(cmdline);
  if (args.size() != 2) {
    return std::unexpected(ParseStatus::kInvalidNumArgs);
  }

  auto time_val = ParseTimeValue(args[1]);
  if (!time_val) {
    return std::unexpected(time_val.error());
  }

  return TimeoutCmd(*time_val);
}

ExecStatus RexmtCmd::Execute([[gnu::unused]] ClientState& config) {
  return ExecStatus::kNotImplemented;
}

std::expected<RexmtCmd, ParseStatus> RexmtCmd::Create(
    std::string_view cmdline) {
  TokenList args = Tokenize(cmdline);
  if (args.size() != 2) {
    return std::unexpected(ParseStatus::kInvalidNumArgs);
  }

  auto time_val = ParseTimeValue(args[1]);
  if (!time_val) {
    return std::unexpected(time_val.error());
  }

  return RexmtCmd(*time_val);
}

ExecStatus QuitCmd::Execute([[gnu::unused]] ClientState& config) {
  return ExecStatus::kNotImplemented;
}

std::expected<QuitCmd, ParseStatus> QuitCmd::Create() { return QuitCmd(); }

ExecStatus HelpCmd::Execute([[gnu::unused]] ClientState& config) {
  return ExecStatus::kNotImplemented;
}

std::expected<HelpCmd, ParseStatus> HelpCmd::Create(std::string_view cmdline) {
  TokenList args = Tokenize(cmdline);
  if (args.size() != 2) {
    return std::unexpected(ParseStatus::kInvalidNumArgs);
  }

  return HelpCmd(args[1]);
}

}  // namespace client
}  // namespace tftp
