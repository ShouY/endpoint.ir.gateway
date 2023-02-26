
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
#include <WiFi.h>

#define LED_BRIGHTNESS 255

#include <nvs_flash.h>

#include <io_manager.hpp>

#include "SerialTerminal.hpp"
#include "cli_command/basic.hpp"
#include "cli_command/ir_gateway.hpp"
#include "cli_command/mqtt.hpp"
#include "cli_command/network.hpp"
#include "mqtt_client.h"

using namespace my::arduino;

maschinendeck::SerialTerminal Terminal;

void initMainCli() {
  // 设置终端
  Terminal.add("echo", terminal_command::echo, "echo parameters");
  Terminal.add("reboot", terminal_command::reboot, "reboot MCU");
  // Terminal.add("blink", terminal_command::blink, "blink onboard LED");
  Terminal.add("netstat", terminal_command::netstat, "show network status");
  // Terminal.add("secho1", terminal_command::serialEcho, "echo proxy of
  // serial1");
  Terminal.add("emit", terminal_command::remote_emit, "send IR signal ");
  Terminal.add("learn", terminal_command::remote_learn, " laearn IR signal");
  Terminal.add("ir_status", terminal_command::remote_status,
               "show IR used status");
  Terminal.add("mqtt_connect", terminal_command::mqtt_connect,
               "connect to mqtt server");
  Terminal.add("publish", terminal_command::mqtt_public,
               "publish message to server");
  Terminal.add("subscribe", terminal_command::mqtt_subscribe,
               "subscribe a topic");
  Terminal.init(&Serial);
}

void setup() {
  // 初始化固件
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);

  Serial1.begin(9600);
  delay(100);

  Serial.printf("init serial1 success: %d\n", Serial1.availableForWrite());

  using my::arduino::io::defaultStream;
  my::arduino::terminal_command::IRCtrl.setLogger(&defaultStream());

  // Initialize NVS
  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES ||
      err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    // NVS partition was truncated and needs to be erased
    // Retry nvs_flash_init
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }
  ESP_ERROR_CHECK(err);
  defaultStream().printf("nvs init result: %d\n", err);

  initMainCli();
  defaultStream().println("Terminal init finish");

  // 默认连接网络
  initNetwork(defaultStream());

  // 初始化MQTT
  my::arduino::client::mqtt::init();
}

void loop() {
  // Read MQTT event
  my::arduino::client::mqtt::GetMQTTCli().loop();

  // Terminla loop
  Terminal.loop();
}
