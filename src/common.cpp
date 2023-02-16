#include "common.hpp"

#include "ESP32WifiCLI.hpp"

namespace my {
namespace arduino {

Stream& defaultStream() { return Serial; }

void connectDefaultNetwork(Stream& stream) {
  wcli.begin(nullptr);
  stream.printf("network begin\n");
  wcli.setSSID("LINK_0BFD");
  wcli.setPASW("qwertyui");

  stream.printf("network start connect\n");
  wcli.connect();
  stream.printf("network end connect, success = %d\n",
                WiFi.status() == WL_CONNECTED);
  // wcli.begin(nullptr);

  auto status = WiFi.status();
  stream.printf("WiFi status = %d\n", status);
}

}  // namespace arduino
}  // namespace my
