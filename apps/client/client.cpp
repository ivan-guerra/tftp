#include <getopt.h>

#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include "client/cmd_processor.h"
#include "client/config.h"

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
  std::cout << "\t-l, --literal-mode\n\t\tinterpret the ':' character literally"
            << std::endl;
  std::cout << "\t-h, --help\n\t\tprint this help message" << std::endl;
}

int main(int argc, char** argv) {
  /* TODO: Add args for timeout and retxmt timeout. */
  const std::vector<struct option> kLongOpts{
      {"hostname", required_argument, 0, 'n'},
      {"mode", required_argument, 0, 'm'},
      {"port", required_argument, 0, 'p'},
      {"port-range", required_argument, 0, 'R'},
      {"literal-mode", no_argument, 0, 'l'},
      {"help", no_argument, 0, 'h'},
      {0, 0, 0, 0},
  };

  std::string hostname = "localhost";
  std::string mode = "ascii";
  std::string port_range = "2048:65535";
  bool literal_mode = false;

  int opt = 0;
  int long_index = 0;
  while ((opt = ::getopt_long(argc, argv, "n:m:p:R:lh", &kLongOpts[0],
                              &long_index)) != -1) {
    switch (opt) {
      case 'n':
        hostname = optarg;
        break;
      case 'm':
        mode = optarg;
        break;
      case 'p':
        port_range = std::string(optarg) + ":" + std::string(optarg);
        break;
      case 'R':
        port_range = optarg;
        break;
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

  auto config =
      tftp::client::Config::Create(mode, port_range, literal_mode, hostname);
  if (!config) {
    PrintErrAndExit(tftp::client::Config::kRetCodeToStr[config.error()]);
  }

  tftp::client::CmdProcessor processor(*config);
  processor.Run();

  std::exit(EXIT_SUCCESS);
}
