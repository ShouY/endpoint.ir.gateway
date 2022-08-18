#ifndef MY_TERMINAL_COMMAND_BASIC_HPP

#include <Arduino.h>

#include "SerialTerminal.hpp"

namespace my {
namespace arduino {
namespace terminal_command {

void blink(String opts) {
  std::pair<String, String> operands =
      maschinendeck::SerialTerminal::ParseCommand(opts);
  int times = operands.first.toInt();
  int miliseconds = operands.second.toInt();
  for (int i = 0; i < times; i++) {
    digitalWrite(LED_BUILTIN, LED_BRIGHTNESS);
    delay(miliseconds);
    digitalWrite(LED_BUILTIN, LOW);
    delay(miliseconds);
  }
}

void echo(String opts) {
  String echo = maschinendeck::SerialTerminal::ParseArgument(opts);
  Serial.println("\r\nmsg: " + echo);
}

void reboot(String opts) { ESP.restart(); }
}  // namespace terminal_command
}  // namespace arduino
}  // namespace my

#endif  // MY_TERMINAL_COMMAND_BASIC_HPP
