#include <getopt.h>

#include <cstdlib>
#include <iostream>
#include <string_view>
#include <vector>

#include "client/cmd_processor.h"
#include "client/config.h"
#include "common/parse.h"
#include "common/types.h"

static void PrintErrAndExit(std::string_view err_msg) {
  std::cerr << "error: " << err_msg << std::endl;
  std::exit(EXIT_FAILURE);
}

static void PrintUsage() {
  std::cout << "usage: tftpc [OPTION]..." << std::endl;
  std::cout << "trivial transfer protocol client" << std::endl;
  std::cout
      << "\t-n, --hostname HOSTNAME\n\t\thostname as either an IPv4 address or "
         "a domain name to be resolved\n\t\tby DNS"
      << std::endl;
  std::cout << "\t-m, --mode MODE\n\t\ttransfer mode one of 'ascii' or 'binary'"
            << std::endl;
  std::cout << "\t-p, --port PORTNUM\n\t\tthis client's source port"
            << std::endl;
  std::cout << "\t-R, --port-range PORT_RANGE\n\t\ta range of ports this "
               "client can use "
               "for tx/rx, must be in the\n\t\tformat <START_PORT>:<END_PORT>"
            << std::endl;
  std::cout << "\t-t, --timeout TOTAL_TRANS_TIMEOUT\n\t\ttotal transmission "
               "time in seconds"
            << std::endl;
  std::cout << "\t-r, --rexmt-timeout REXMT_TIMEOUT\n\t\tper packet "
               "retransmission time in seconds"
            << std::endl;
  std::cout << "\t-l, --literal-mode\n\t\tinterpret the ':' character literally"
            << std::endl;
  std::cout << "\t-h, --help\n\t\tprint this help message" << std::endl;
}

int main(int argc, char** argv) {
  const std::vector<struct option> kLongOpts{
      {"hostname", required_argument, 0, 'n'},
      {"mode", required_argument, 0, 'm'},
      {"port", required_argument, 0, 'p'},
      {"port-range", required_argument, 0, 'R'},
      {"timeout", required_argument, 0, 't'},
      {"rexmt-timeout", required_argument, 0, 'r'},
      {"literal-mode", no_argument, 0, 'l'},
      {"help", no_argument, 0, 'h'},
      {0, 0, 0, 0},
  };

  tftp::Hostname hostname = "localhost";
  tftp::Mode mode = tftp::SendMode::kNetAscii;
  tftp::PortRange port_range = {.start = 2048, .end = 65535};
  tftp::Seconds timeout = 60;
  tftp::Seconds rexmt_timeout = 10;
  bool literal_mode = false;

  int opt = 0;
  int long_index = 0;
  while ((opt = ::getopt_long(argc, argv, "n:m:p:R:t:r:lh", &kLongOpts[0],
                              &long_index)) != -1) {
    switch (opt) {
      case 'n':
        hostname = optarg;
        break;
      case 'm': {
        auto parsed_mode = tftp::ParseMode(optarg);
        if (!parsed_mode) {
          PrintErrAndExit(tftp::kParseStatusToStr[parsed_mode.error()]);
        }
        mode = *parsed_mode;
        break;
      }
      case 'p': {
        auto parsed_port = tftp::ParsePort(optarg);
        if (!parsed_port) {
          PrintErrAndExit(tftp::kParseStatusToStr[parsed_port.error()]);
        }
        port_range = {.start = *parsed_port, .end = *parsed_port};
        break;
      }
      case 'R': {
        auto parsed_port_rng = tftp::ParsePortRange(optarg);
        if (!parsed_port_rng) {
          PrintErrAndExit(tftp::kParseStatusToStr[parsed_port_rng.error()]);
        }
        port_range = *parsed_port_rng;
        break;
      }
      case 't': {
        auto parsed_timeout = tftp::ParseTimeValue(optarg);
        if (!parsed_timeout) {
          PrintErrAndExit(tftp::kParseStatusToStr[parsed_timeout.error()]);
        }
        timeout = *parsed_timeout;
        break;
      }
      case 'r': {
        auto parsed_timeout = tftp::ParseTimeValue(optarg);
        if (!parsed_timeout) {
          PrintErrAndExit(tftp::kParseStatusToStr[parsed_timeout.error()]);
        }
        rexmt_timeout = *parsed_timeout;
        break;
      }
      case 'l':
        literal_mode = true;
        break;
      case 'h':
        PrintUsage();
        std::exit(EXIT_SUCCESS);
        break;
      case '?':
        std::cerr << "run 'tftpc --help' for usage info" << std::endl;
        std::exit(EXIT_FAILURE);
    }
  }

  tftp::client::Config conf(mode, port_range, literal_mode, hostname, timeout,
                            rexmt_timeout);
  tftp::client::CmdProcessor processor(conf);
  processor.Run();

  std::exit(EXIT_SUCCESS);
}
