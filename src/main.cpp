
/*********************************************************************
 This sample file is part of the esp32-wifi-cli:
 Copyright (c) 2022, @hpsaturn, Antonio Vanegas
 https://hpsaturn.com, All rights reserved.
 https://github.com/hpsaturn/esp32-wifi-cli

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, version 3.

 This program is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
 *********************************************************************/

// #define NO_GLOBAL_ESP32WIFICLI
#define ST_FLAG_NOHELP

#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>

#include "ESP32WifiCLI.hpp"
#include "SerialTerminal.hpp"

maschinendeck::SerialTerminal Terminal;
PubSubClient PSCli;

Stream& getDefaultStream() { return Serial; }

/*********************************************************************
 * User defined commands. Example: suspend, blink, reboot, etc.
 ********************************************************************/
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

void connectDefaultNetwork(Stream& stream) {
  wcli.begin(&stream);
  wcli.setSSID("TP-LINK_0BFD");
  wcli.setPASW("qwertyui");
  wcli.connect();
  wcli.begin(nullptr);
}

void netstat(String opt) {
  wcli.begin(&getDefaultStream());
  wcli.status();
  wcli.begin(nullptr);
}

void setup() {
  // 初始化固件
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  delay(100);

  // 设置终端
  Terminal.add("echo", echo, "echo parameters");
  Terminal.add("reboot", reboot, "reboot MCU");
  Terminal.add("blink", blink, "blink");
  Terminal.add("netstat", netstat, "show WiFi connect status");

  Terminal.init(&Serial);
  getDefaultStream().println("Terminal start finish");

  // 默认连接网络
  connectDefaultNetwork(getDefaultStream());
}

void loop() {
  // Terminla loop
  Terminal.loop();
}
