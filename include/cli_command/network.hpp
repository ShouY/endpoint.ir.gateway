#ifndef TERMINAL_COMMAND_NETWORK_HPP

#include "ESP32WifiCLI.hpp"
#include "SerialTerminal.hpp"
#include "common.hpp"

namespace my {
namespace arduino {
namespace terminal_command {

void netstat(String opt) { wcli.status(defaultStream()); }

}  // namespace terminal_command
}  // namespace arduino
}  // namespace my

#endif  // TERMINAL_COMMAND_NETWORK_HPP
