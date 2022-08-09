
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

#include <Arduino.h>
// #include <M5Atom.h>

// #include <AsyncTCP.h>
#include <PubSubClient.h>
#include <WiFi.h>

#include "ESP32WifiCLI.hpp"
#include "SerialTerminal.hpp"

maschinendeck::SerialTerminal Terminal;

/*********************************************************************
 * User defined commands. Example: suspend, blink, reboot, etc.
 ********************************************************************/
void blink(String opts) {
  std::pair<String, String> operands =
      maschinendeck::SerialTerminal::ParseCommand(opts);
  int times = operands.first.toInt();
  int miliseconds = operands.second.toInt();
  for (int i = 0; i < times; i++) {
    // M5.dis.fillpix(0xaaff00);  // set LED to green
    digitalWrite(LED_BUILTIN, LED_BRIGHTNESS);
    delay(miliseconds);
    // M5.dis.fillpix(0xff0000);  // set LED to green
    digitalWrite(LED_BUILTIN, LOW);
    delay(miliseconds);
  }
}

void echo(String opts) {
  String echo = maschinendeck::SerialTerminal::ParseArgument(opts);
  Serial.println("\r\nmsg: " + echo);
}

void reboot(String opts) { ESP.restart(); }

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  Terminal.add("echo", echo, "echo parameters");
  Terminal.add("reboot", reboot, "reboot MCU");
  Terminal.add("blink", blink, "blink");

  Serial.begin(115200);
  Terminal.init(&Serial);
  while (!Terminal.is_open()) {
    delay(100);
  }
}

void loop() {
  // Terminla loop
  Terminal.loop();
}
