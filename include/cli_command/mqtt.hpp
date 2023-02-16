#ifndef TERMINAL_COMMAND_NETWORK_HPP

#include <PubSubClient.h>
#include <WiFiClient.h>

#include "../common.hpp"
#include "SerialTerminal.hpp"

namespace my {
namespace arduino {
namespace terminal_command {

static WiFiClient _client;
static PubSubClient PSCli;

namespace parser {

int parsePort(String num) {
  auto& stream = defaultStream();
  // stream.printf("Parse %s\n", num.c_str());
  int val = 0;
  for (auto c : num) {
    if (!isdigit(c)) {
      return -1;
    }
    val *= 10;
    val += c - '0';
    if (val >= 65536) {
      return -2;
    }
  }
  return val;
}
}  // namespace parser

void mqtt_connect(String opts) {
  auto& stream = defaultStream();
  opts.trim();
  if (opts.length() == 0) {
    stream.println("parameters size error");
    return;
  }

  std::string address_port(opts.begin(), opts.end());
  IPAddress address;
  int port = 1883;  // default 1883

  {  // Parse
    auto address_end = address_port.find_first_of(' ', 0);
    if (address_end == std::string::npos) {
      address_end = address_port.length();
    }
    auto addressParsed =
        address.fromString(String(address_port.c_str(), address_end));
    if (!addressParsed) {
      stream.printf("address parse failed\n");
      return;
    }
    // parse port
    if (address_end != address_port.length()) {  // 指定了目标端口号
      auto port_begin = address_port.find_first_not_of(' ', address_end + 1);
      if (port_begin == std::string::npos) {
        stream.println("port not found");
        return;
      }
      auto port_end = address_port.find_first_of(' ', port_begin);
      if (port_end != std::string::npos) {
        stream.println("parameters size error");
        return;
      }
      port_end = address_port.length();
      port = parser::parsePort(
          String(address_port.c_str() + port_begin, port_end - port_begin));
      if (port < 0) {
        stream.printf("invalid port");
        return;
      }
    }
  }
  // set server and client
  stream.printf("MQTT connect to %s:%d\n", address.toString(), port);
  PSCli.setServer(address, port).setClient(_client);  // client is global object
  bool connected = PSCli.connect("esp32-ir");
  stream.printf("MQTT status = %d, connected = %d", PSCli.state(), connected);
}

void mqtt_public(String opt) {
  auto& stream = defaultStream();
  if (!PSCli.connected()) {
    auto connected = PSCli.connect(opt.c_str());
    stream.println("PSCli is not connected\n");
    return;
  }
  auto sent = PSCli.publish("hello", opt.c_str(), opt.length());
  stream.printf("PSCli publish topic \"hello\", send result: %d\n", sent);
}

}  // namespace terminal_command
}  // namespace arduino
}  // namespace my

#endif  // TERMINAL_COMMAND_NETWORK_HPP
