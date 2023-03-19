#ifndef MY_ARDUINO_TERMINAL_COMMAND_NETWORK_HPP
#define MY_ARDUINO_TERMINAL_COMMAND_NETWORK_HPP

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

}  // namespace terminal_command
}  // namespace arduino
}  // namespace my

#endif  // MY_ARDUINO_TERMINAL_COMMAND_NETWORK_HPP
