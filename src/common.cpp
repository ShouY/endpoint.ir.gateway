#include "common.hpp"

#include "ESP32WifiCLI.hpp"

namespace my {
namespace arduino {

Stream& defaultStream() { return Serial; }

void initNetwork(Stream& stream) {
  String ssid("LINK_0BFD");
  String passwd("qwertyui");
  wcli.begin(nullptr);
  wcli.setSSID(ssid);
  wcli.setPASW(passwd);
  stream.printf("network start connect to \"%s\"\n", ssid);
  wcli.connect();
  bool isConnected = WiFi.status() == WL_CONNECTED;
  if (isConnected) {
    stream.printf("WiFi connect to %s success\n", ssid);
    wcli.printWifiStatus(stream);
    stream.printf("=================================\n\n");
  } else {
    stream.printf("WiFi connect to %s failed\n", ssid);
  }
}

}  // namespace arduino
}  // namespace my
