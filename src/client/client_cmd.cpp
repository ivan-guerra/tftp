#include "client/client_cmd.h"

#include <string_view>

#include "client/config.h"

namespace tftp {
namespace client {

ExecStatus ConnectCmd::Execute([[gnu::unused]] client::Config& config) {
  return ExecStatus::kSuccess;
}

ParseStatus ConnectCmd::Parse([[gnu::unused]] std::string_view cmdline) {
  return ParseStatus::kSuccess;
}

ExecStatus GetCmd::Execute([[gnu::unused]] client::Config& config) {
  return ExecStatus::kSuccess;
}

ParseStatus GetCmd::Parse([[gnu::unused]] std::string_view cmdline) {
  return ParseStatus::kSuccess;
}

ExecStatus PutCmd::Execute([[gnu::unused]] client::Config& config) {
  return ExecStatus::kSuccess;
}

ParseStatus PutCmd::Parse([[gnu::unused]] std::string_view cmdline) {
  return ParseStatus::kSuccess;
}

ExecStatus LiteralCmd::Execute([[gnu::unused]] client::Config& config) {
  return ExecStatus::kSuccess;
}

ParseStatus LiteralCmd::Parse([[gnu::unused]] std::string_view cmdline) {
  return ParseStatus::kSuccess;
}

ExecStatus ModeCmd::Execute([[gnu::unused]] client::Config& config) {
  return ExecStatus::kSuccess;
}

ParseStatus ModeCmd::Parse([[gnu::unused]] std::string_view cmdline) {
  return ParseStatus::kSuccess;
}

ExecStatus StatusCmd::Execute([[gnu::unused]] client::Config& config) {
  return ExecStatus::kSuccess;
}

ParseStatus StatusCmd::Parse([[gnu::unused]] std::string_view cmdline) {
  return ParseStatus::kSuccess;
}

ExecStatus TimeoutCmd::Execute([[gnu::unused]] client::Config& config) {
  return ExecStatus::kSuccess;
}

ParseStatus TimeoutCmd::Parse([[gnu::unused]] std::string_view cmdline) {
  return ParseStatus::kSuccess;
}

ExecStatus RexmtCmd::Execute([[gnu::unused]] client::Config& config) {
  return ExecStatus::kSuccess;
}

ParseStatus RexmtCmd::Parse([[gnu::unused]] std::string_view cmdline) {
  return ParseStatus::kSuccess;
}

ExecStatus QuitCmd::Execute([[gnu::unused]] client::Config& config) {
  return ExecStatus::kSuccess;
}

ParseStatus QuitCmd::Parse([[gnu::unused]] std::string_view cmdline) {
  return ParseStatus::kSuccess;
}

ExecStatus HelpCmd::Execute([[gnu::unused]] client::Config& config) {
  return ExecStatus::kSuccess;
}

ParseStatus HelpCmd::Parse([[gnu::unused]] std::string_view cmdline) {
  return ParseStatus::kSuccess;
}

}  // namespace client
}  // namespace tftp
