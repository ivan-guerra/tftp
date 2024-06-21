#ifndef CMD_PROCESSOR_H_
#define CMD_PROCESSOR_H_

#include <boost/asio.hpp>
#include <expected>
#include <string_view>

#include "client/cmd.h"

namespace tftp {
namespace client {

class CmdProcessor {
 public:
  explicit CmdProcessor(ClientStatePtr state)
      : state_(state), worker_thrds_(std::thread::hardware_concurrency()) {}

  ~CmdProcessor() { worker_thrds_.join(); }

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
  void CmdExecutor(ClientStatePtr state, int cmd_num);

  ClientStatePtr state_;
  boost::asio::thread_pool worker_thrds_;
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
