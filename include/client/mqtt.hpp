#ifndef MY_ARDUINO_CLIENT_MQTT_HPP
#define MY_ARDUINO_CLIENT_MQTT_HPP

#include <PubSubClient.h>
#include <WiFiClient.h>

namespace my {
namespace arduino {
namespace client {
namespace mqtt {

static WiFiClient _mqtt_underlay_client;
static PubSubClient _MQTT_cli;

constexpr uint16_t MQTT_KEEP_ALIVE = 60;

void init() {
  _MQTT_cli.setClient(_mqtt_underlay_client).setKeepAlive(MQTT_KEEP_ALIVE);
}

PubSubClient& GetMQTTCli() { return _MQTT_cli; }

}  // namespace mqtt
}  // namespace client
}  // namespace arduino
}  // namespace my

#endif MY_ARDUINO_CLIENT_MQTT_HPP
