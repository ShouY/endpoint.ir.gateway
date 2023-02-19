#ifndef MY_ARDUINO_TERMINAL_COMMAND_IR_GATEWAY_HPP
#define MY_ARDUINO_TERMINAL_COMMAND_IR_GATEWAY_HPP
#include <Arduino.h>

#include "../firmware/irk02.hpp"

namespace my {
namespace arduino {
namespace terminal_command {

static typename firmware::controller::IRControllor IRCtrl;

void remote_emit(String opts) {
  auto key = opts.toInt();
  if (key >= ::my::firmware::controller::IRControllor::MIN_INVALID_KEY) {
    Serial.printf("Emit failed, key %d is out of range\n", key);
  }
  auto success = IRCtrl.emit(key);
  Serial.printf("Emit %d >> %s", key, success ? "success" : "failed");
}

void remote_learn(String opts) {
  auto key = opts.toInt();
  auto success = IRCtrl.learn(key);
  Serial.printf("Learn %d, code = %d\n", key, success);
}

void remote_status(String opts) {
  auto key = opts.toInt();
  auto used = IRCtrl.status(key);
  Serial.printf("IR Key %d is used: ", key);
  Serial.println(used);
}

}  // namespace terminal_command
}  // namespace arduino
}  // namespace my

#endif
