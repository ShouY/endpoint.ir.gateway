#ifndef MY_ARDUINO_MQTT_CLIENT_HPP
#define MY_ARDUINO_MQTT_CLIENT_HPP

#include <PubSubClient.h>
#include <WiFiClient.h>

namespace my {
namespace arduino {
namespace mqtt {
namespace client {

namespace underlay {
static WiFiClient _mqtt_underlay_client;
static Client& _client = _mqtt_underlay_client;
}  // namespace underlay

static PubSubClient _MQTT_cli;

constexpr uint16_t MQTT_KEEP_ALIVE = 60;

void init() {
  _MQTT_cli.setClient(underlay::_client).setKeepAlive(MQTT_KEEP_ALIVE);
}

PubSubClient& GetMQTTCli() { return _MQTT_cli; }

}  // namespace client
}  // namespace mqtt
}  // namespace arduino
}  // namespace my

#endif  // MY_ARDUINO_MQTT_CLIENT_HPP
