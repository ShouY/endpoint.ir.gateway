#ifndef TERMINAL_COMMAND_NETWORK_HPP

#include "ESP32WifiCLI.hpp"
#include "SerialTerminal.hpp"
#include "common.hpp"

namespace my {
namespace arduino {
namespace terminal_command {

void netstat(String opt) {
  wcli.begin(&defaultStream());
  wcli.status();
  wcli.begin(nullptr);
}

}  // namespace terminal_command
}  // namespace arduino
}  // namespace my

#endif  // TERMINAL_COMMAND_NETWORK_HPP
