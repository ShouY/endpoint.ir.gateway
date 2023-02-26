#ifndef TERMINAL_COMMAND_NETWORK_HPP

#include <io_manager.hpp>

#include "ESP32WifiCLI.hpp"
#include "SerialTerminal.hpp"

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

#endif  // TERMINAL_COMMAND_NETWORK_HPP
