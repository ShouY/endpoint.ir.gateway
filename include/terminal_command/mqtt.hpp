#ifndef TERMINAL_COMMAND_NETWORK_HPP

#include <PubSubClient.h>
#include <WiFiClient.h>

#include "../hardware_common.hpp"
#include "SerialTerminal.hpp"

namespace my {
namespace arduino {
namespace terminal_command {

static WiFiClient _client;
static PubSubClient PSCli("192.168.0.104", 1883, _client);

void mqtt_connect(String opt) {
  auto& stream = getDefaultStream();
  stream.printf("connect to: [%s]\n", opt.c_str());
  auto connected = PSCli.connect(opt.c_str());
  stream.printf("PSCli connect: %d\n", connected);
}

void mqtt_public(String opt) {
  auto& stream = getDefaultStream();
  if (!PSCli.connected()) {
    stream.println("PSCli is not connected\n");
    return;
  }
  auto sent = PSCli.publish("hello", opt.c_str(), opt.length());
  stream.printf("PSCli publis ·hello· result: %d\n", sent);
}

}  // namespace terminal_command
}  // namespace arduino
}  // namespace my

#endif  // TERMINAL_COMMAND_NETWORK_HPP
