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
  explicit CmdProcessor(const Config& conf) : conf_(conf) {}

  ~CmdProcessor() = default;
  CmdProcessor() = delete;
  CmdProcessor(const CmdProcessor&) = delete;
  CmdProcessor(CmdProcessor&&) = delete;
  CmdProcessor& operator=(const CmdProcessor&) = delete;
  CmdProcessor& operator=(CmdProcessor&&) = delete;

  void Run();

 private:
  template <typename T>
  std::expected<CmdPtr, ParseStatus> CreateCmd() const;
  template <typename T>
  std::expected<CmdPtr, ParseStatus> CreateCmd(std::string_view cmdline) const;
  std::expected<CmdPtr, ParseStatus> LoadCmd(std::string_view cmdline) const;

  void PrintError(std::string_view err_msg) const;

  Config conf_;
};

template <typename T>
std::expected<CmdPtr, ParseStatus> CmdProcessor::CreateCmd() const {
  return T::Create();
}

template <typename T>
std::expected<CmdPtr, ParseStatus> CmdProcessor::CreateCmd(
    std::string_view cmdline) const {
  return T::Create(cmdline);
}

}  // namespace client
}  // namespace tftp

#endif
