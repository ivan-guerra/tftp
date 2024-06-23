#ifndef CMD_PROCESSOR_H_
#define CMD_PROCESSOR_H_

#include <expected>
#include <string_view>

#include "client/cmd.h"
#include "client/config.h"

namespace tftp {
namespace client {

class CmdProcessor {
 public:
  explicit CmdProcessor(ConfigPtr conf) : conf_(conf) {}

  ~CmdProcessor() = default;
  CmdProcessor() = delete;
  CmdProcessor(const CmdProcessor&) = delete;
  CmdProcessor(CmdProcessor&&) = delete;
  CmdProcessor& operator=(const CmdProcessor&) = delete;
  CmdProcessor& operator=(CmdProcessor&&) = delete;

  void Exec(std::string_view cmdline);

 private:
  template <typename T>
  std::expected<CmdPtr, ParseStatus> LoadCmd() const;
  template <typename T>
  std::expected<CmdPtr, ParseStatus> LoadCmd(std::string_view cmdline) const;

  void PrintError(std::string_view err_msg) const;

  ConfigPtr conf_;
};

template <typename T>
std::expected<CmdPtr, ParseStatus> CmdProcessor::LoadCmd() const {
  return T::Create();
}

template <typename T>
std::expected<CmdPtr, ParseStatus> CmdProcessor::LoadCmd(
    std::string_view cmdline) const {
  return T::Create(cmdline);
}

}  // namespace client
}  // namespace tftp

#endif