#ifndef MY_ARDUINO_TERMINAL_COMMAND_NETWORK_HPP
#define MY_ARDUINO_TERMINAL_COMMAND_NETWORK_HPP

#include <ESP32Ping.h>

#include <ESP32WifiCLI.hpp>
#include <SerialTerminal.hpp>
#include <io_manager.hpp>

namespace my {
namespace arduino {
namespace terminal_command {

void netstat(String opt) {
  using ::my::arduino::io::defaultStream;
  wcli.status(defaultStream());
}

void ping(String opt) {
  using ::my::arduino::io::defaultStream;

  opt.trim();
  IPAddress address;
  bool isAddress = address.fromString(opt);
  if (!isAddress) {
    defaultStream().printf("invalid address: '%s'", opt);
    return;
  }
  if (Ping.ping(address)) {
    defaultStream().printf("Ping '%s' success", opt);
  } else {
    defaultStream().printf("Ping '%s' failed", opt);
  }
}

}  // namespace terminal_command
}  // namespace arduino
}  // namespace my

#endif  // MY_ARDUINO_TERMINAL_COMMAND_NETWORK_HPP
