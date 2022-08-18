#ifndef TERMINAL_COMMAND_NETWORK_HPP

#include "../hardware_common.hpp"
#include "ESP32WifiCLI.hpp"
#include "SerialTerminal.hpp"

namespace my {
namespace arduino {
namespace terminal_command {

void netstat(String opt) {
  wcli.begin(&getDefaultStream());
  wcli.status();
  wcli.begin(nullptr);
}

}  // namespace terminal_command
}  // namespace arduino
}  // namespace my

#endif  // TERMINAL_COMMAND_NETWORK_HPP
