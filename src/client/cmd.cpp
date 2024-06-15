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

#include "client/config.h"
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

ExecStatus ConnectCmd::Execute([[gnu::unused]] client::Config& config) {
  return ExecStatus::kSuccess;
}

ParseStatus ConnectCmd::Parse(std::string_view cmdline) {
  TokenList args = Tokenize(cmdline);
  if (args.size() < 2 || args.size() > 3) {
    return ParseStatus::kInvalidNumArgs;
  }

  host_ = args[1];
  if (args.size() == 3) {
    if (!IsPositiveNum(args[2])) {
      return ParseStatus::kInvalidPortNum;
    }

    uint64_t port_tmp = std::stoull(args[2]);
    if (port_tmp > std::numeric_limits<uint16_t>::max()) {
      return ParseStatus::kInvalidPortNum;
    }

    port_ = static_cast<uint16_t>(port_tmp);
  }

  return ParseStatus::kSuccess;
}

ExecStatus GetCmd::Execute([[gnu::unused]] client::Config& config) {
  return ExecStatus::kSuccess;
}

ParseStatus GetCmd::Parse(std::string_view cmdline) {
  TokenList args = Tokenize(cmdline);
  if (args.size() <= 1) {
    return ParseStatus::kInvalidNumArgs;
  }
  args.erase(args.begin()); /* Erase the command code. */

  if (args.size() == 1) { /* Fetch a single file. */
    files_ = {args[0]};
  }

  if (args.size() == 2) { /* Fetch remotefile to localfile */
    remote_file_ = args[0];
    local_file_ = args[1];
  }

  if (args.size() > 2) { /* Fetching a set of files. */
    files_ = args;
  }

  return ParseStatus::kSuccess;
}

ExecStatus PutCmd::Execute([[gnu::unused]] client::Config& config) {
  return ExecStatus::kSuccess;
}

ParseStatus PutCmd::Parse(std::string_view cmdline) {
  TokenList args = Tokenize(cmdline);
  if (args.size() <= 1) {
    return ParseStatus::kInvalidNumArgs;
  }
  args.erase(args.begin()); /* Erase the command code. */

  if (args.size() == 1) { /* Transfer a single file. */
    files_ = {args[0]};
  }

  if (args.size() == 2) { /* Transfer remotefile to localfile */
    local_file_ = args[0];
    remote_file_ = args[1];
  }

  if (args.size() > 2) { /* Transferring a set of files to a remote dir. */
    remote_dir_ = args.back();
    files_ = FileList(args.cbegin(), args.cbegin() + args.size() - 1);
  }

  return ParseStatus::kSuccess;
}

ExecStatus LiteralCmd::Execute([[gnu::unused]] client::Config& config) {
  return ExecStatus::kSuccess;
}

ParseStatus LiteralCmd::Parse([[gnu::unused]] std::string_view cmdline) {
  /* Nothing to parse. */
  return ParseStatus::kSuccess;
}

ExecStatus ModeCmd::Execute([[gnu::unused]] client::Config& config) {
  return ExecStatus::kSuccess;
}

ParseStatus ModeCmd::Parse(std::string_view cmdline) {
  TokenList args = Tokenize(cmdline);
  if (args.size() != 2) {
    return ParseStatus::kInvalidNumArgs;
  }

  std::string mode_lower(args[1].size(), 0);
  std::transform(args[1].cbegin(), args[1].cend(), mode_lower.begin(),
                 [](char c) { return std::tolower(c); });

  if ((mode_lower != SendMode::kNetAscii) && (mode_lower != SendMode::kOctet)) {
    return ParseStatus::kInvalidMode;
  }

  mode_ = mode_lower;

  return ParseStatus::kSuccess;
}

ExecStatus StatusCmd::Execute([[gnu::unused]] client::Config& config) {
  return ExecStatus::kSuccess;
}

ParseStatus StatusCmd::Parse([[gnu::unused]] std::string_view cmdline) {
  /* Nothing to parse. */
  return ParseStatus::kSuccess;
}

ExecStatus TimeoutCmd::Execute([[gnu::unused]] client::Config& config) {
  return ExecStatus::kSuccess;
}

ParseStatus TimeoutCmd::Parse(std::string_view cmdline) {
  TokenList args = Tokenize(cmdline);
  if (args.size() != 2) {
    return ParseStatus::kInvalidNumArgs;
  }

  auto time_val = ParseTimeValue(args[1]);
  if (!time_val) {
    return time_val.error();
  }

  timeout_ = *time_val;

  return ParseStatus::kSuccess;
}

ExecStatus RexmtCmd::Execute([[gnu::unused]] client::Config& config) {
  return ExecStatus::kSuccess;
}

ParseStatus RexmtCmd::Parse(std::string_view cmdline) {
  TokenList args = Tokenize(cmdline);
  if (args.size() != 2) {
    return ParseStatus::kInvalidNumArgs;
  }

  auto time_val = ParseTimeValue(args[1]);
  if (!time_val) {
    return time_val.error();
  }

  rextm_timeout_ = *time_val;

  return ParseStatus::kSuccess;
}

ExecStatus QuitCmd::Execute([[gnu::unused]] client::Config& config) {
  return ExecStatus::kSuccess;
}

ParseStatus QuitCmd::Parse([[gnu::unused]] std::string_view cmdline) {
  /* Nothing to parse. */
  return ParseStatus::kSuccess;
}

ExecStatus HelpCmd::Execute([[gnu::unused]] client::Config& config) {
  return ExecStatus::kSuccess;
}

ParseStatus HelpCmd::Parse(std::string_view cmdline) {
  TokenList args = Tokenize(cmdline);
  if (args.size() != 2) {
    return ParseStatus::kInvalidNumArgs;
  }

  target_cmd_ = args[1];

  return ParseStatus::kSuccess;
}

}  // namespace client
}  // namespace tftp
