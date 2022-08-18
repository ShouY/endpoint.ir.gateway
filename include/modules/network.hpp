
#include "ESP32WifiCLI.hpp"

namespace my {
namespace arduino {

void connectDefaultNetwork(Stream& stream) {
  wcli.begin(&stream);
  wcli.setSSID("TP-LINK_0BFD");
  wcli.setPASW("qwertyui");
  wcli.connect();
  wcli.begin(nullptr);
}

}  // namespace arduino
}  // namespace my
