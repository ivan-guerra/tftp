#include "client/config.h"

#include <cctype>
#include <expected>
#include <memory>
#include <optional>
#include <string_view>

#include "common/parse.h"
#include "common/types.h"

namespace tftp {
namespace client {

std::expected<ConfigPtr, ParseStatus> Config::Create(
    std::string_view mode, std::string_view port_range, bool literal_mode,
    const Hostname& hostname, std::string_view timeout,
    std::string_view rexmt_timeout) {
  auto parsed_mode = ParseMode(mode);
  if (!parsed_mode) {
    return std::unexpected(parsed_mode.error());
  }

  auto parsed_range = ParsePortRange(port_range);
  if (!parsed_range) {
    return std::unexpected(parsed_range.error());
  }

  auto parsed_timeout = ParseTimeValue(timeout);
  if (!parsed_timeout) {
    return std::unexpected(parsed_timeout.error());
  }
  auto parsed_rexmt_timeout = ParseTimeValue(rexmt_timeout);
  if (!parsed_rexmt_timeout) {
    return std::unexpected(parsed_rexmt_timeout.error());
  }

  /* Have to use a raw new here since std::make_shared<> cannot access the
   * private Config constructor. */
  return std::shared_ptr<Config>(
      new Config(*parsed_mode, *parsed_range, literal_mode, hostname,
                 *parsed_timeout, *parsed_rexmt_timeout));
}

std::optional<ParseStatus> Config::SetMode(std::string_view mode) {
  auto parsed_mode = ParseMode(mode);
  if (!parsed_mode) {
    return parsed_mode.error();
  }

  mode_ = *parsed_mode;

  return std::nullopt;
}

std::optional<ParseStatus> Config::SetPortRange(std::string_view port_range) {
  auto parsed_range = ParsePortRange(port_range);
  if (!parsed_range) {
    return parsed_range.error();
  }

  ports_ = *parsed_range;

  return std::nullopt;
}

std::optional<ParseStatus> Config::SetTimeout(std::string_view timeout) {
  auto parsed_timeout = ParseTimeValue(timeout);
  if (!parsed_timeout) {
    return parsed_timeout.error();
  }

  timeout_ = *parsed_timeout;

  return std::nullopt;
}

std::optional<ParseStatus> Config::SetRexmtTimeout(
    std::string_view rexmt_timeout) {
  auto parsed_rexmt_timeout = ParseTimeValue(rexmt_timeout);
  if (!parsed_rexmt_timeout) {
    return parsed_rexmt_timeout.error();
  }

  rexmt_timeout_ = *parsed_rexmt_timeout;

  return std::nullopt;
}

}  // namespace client
}  // namespace tftp
