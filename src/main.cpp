
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

#include "SerialTerminal.hpp"
#include "modules/network.hpp"
#include "terminal_command/basic.hpp"
#include "terminal_command/ir_gateway.hpp"
#include "terminal_command/network.hpp"

using namespace my::arduino;

maschinendeck::SerialTerminal Terminal;
PubSubClient PSCli;

/*********************************************************************
 * User defined commands. Example: suspend, blink, reboot, etc.
 ********************************************************************/

void setup() {
  // 初始化固件
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  Serial1.begin(9600);
  delay(100);

  // 设置终端
  Terminal.add("echo", terminal_command::echo, "echo parameters");
  Terminal.add("reboot", terminal_command::reboot, "reboot MCU");
  Terminal.add("blink", terminal_command::blink, "blink onboard LED");
  Terminal.add("netstat", terminal_command::netstat, "show network status");
  Terminal.add("secho1", terminal_command::serialEcho, "echo proxy of serial1");
  Terminal.add("emit", terminal_command::remote_emit, "send IR signal");

  Terminal.init(&Serial);
  getDefaultStream().println("Terminal start finish");

  // 默认连接网络
  connectDefaultNetwork(getDefaultStream());
}

void loop() {
  // Terminla loop
  Terminal.loop();
}
