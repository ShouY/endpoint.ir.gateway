/* IR Remote Controller IRK02
 *
 * Document:
 * https://item.taobao.com/item.htm?spm=a1z09.2.0.0.67072e8dm58dp9&id=663016557026&_u=m2ep4l0c71f6
 *
 */

#ifndef MY_ARDUINO_FIRMWARE_IRK02_HPP
#define MY_ARDUINO_FIRMWARE_IRK02_HPP

#include <Arduino.h>

#include <numeric>
#include <string>

#include "common.hpp"

#define DEBUG_IR_COMMAND true  // 输出命令调试日志

namespace my {
namespace firmware {
namespace controller {

class IRControllor {
 public:
  static constexpr char HEADER_PREFIX0 = 0x55;
  static constexpr char HEADER_PREFIX1 = 0xAA;

  static constexpr char CMD_GET_STATUS = 0x01;
  static constexpr char CMD_LEARN = 0x02;
  static constexpr char CMD_EMIT = 0x03;
  static constexpr char CMD_BAUD = 0x04;
  static constexpr char CMD_INVALID = 0x04;

  static constexpr char MIN_INVALID_KEY = 48;

  static constexpr char BAUD002400 = 0x00;
  static constexpr char BAUD004800 = 0x01;
  static constexpr char BAUD009600 = 0x02;
  static constexpr char BAUD019200 = 0x03;
  static constexpr char BAUD115200 = 0x04;
  static constexpr char BAUD_invalid = 0x05;

  // 响应信息在返回报文中的起始位置偏移
  static constexpr uint8_t RESPONSE_OFFSET = 3;
  static constexpr char CMD_SUCCESS = 0x01;

  static constexpr uint8_t STATUS_IDLE = 0;
  static constexpr uint8_t STATUS_USED = 1;
  static constexpr uint8_t STATUS_FAILED = 2;
  static constexpr uint8_t STATUS_INVALID_KEY = 3;

 public:
  IRControllor(Print* logger = nullptr) : retry_times_(2), logger_(logger) {}

  bool learn(uint8_t code) {
    auto respLen = sendCmd(CMD_LEARN, code);
    return respLen == 1 && recv_buf_[RESPONSE_OFFSET] == CMD_SUCCESS;
  }

  bool emit(uint8_t code) {
    auto respLen = sendCmd(CMD_EMIT, code);
    return respLen == 1 && recv_buf_[RESPONSE_OFFSET] == CMD_SUCCESS;
  }

  // 查看key是否已经占用
  uint8_t status(int key) {
    if (key >= MIN_INVALID_KEY) {
      return STATUS_INVALID_KEY;
    }
    auto respLen = sendCmd(CMD_GET_STATUS, 0x00);
    Serial.printf("status resp len=%d\n", respLen);
    if (respLen != 8) {
      return STATUS_FAILED;
    }
#ifdef DEBUG_IR_COMMAND
    if (logger_ != nullptr) {
      // 总共8字节表示结果，其中前两个字节的值无效，略过
      constexpr uint8_t STATUS_VALUE_SIZE = 8;
      for (uint8_t i = 2; i < STATUS_VALUE_SIZE; ++i) {
        uint8_t value = recv_buf_[RESPONSE_OFFSET + i];
        for (uint8_t mask = (1 << 7); mask > 0; mask >>= 1) {
          logger_->print(bool(value & mask));
        }
        logger_->print(".");
      }
      logger_->println();
    }
#endif
    char valueByte = recv_buf_[RESPONSE_OFFSET + 8 - key / 8];
    uint8_t bitIndex = key % 8;
    return bitRead(valueByte, bitIndex);  // STATUS_IDLE or STATUS_USED
  }

  bool setLogger(Print* out) {
    this->logger_ = out;
    return true;
  }

 private:
  bool validCode(uint8_t code) const noexcept { return code < 48; }

  // Send command to ir controller. Return zero or positive if success, and
  // negative if failed.
  int sendCmd(char operate, char key) {
    size_t cmd_len;  // 命令的长度
    // check
    switch (operate) {
      case CMD_LEARN:
      case CMD_EMIT:
        if (uint8_t(key) >= MIN_INVALID_KEY) {
          return -1;
        }
        cmd_len = 5;
        break;
      case CMD_GET_STATUS:
        cmd_len = 4;
        key = 0x00;
        break;
      case CMD_BAUD:
        if (uint8_t(key) >= BAUD_invalid) {
          return -1;
        }
        cmd_len = 5;
        break;
      default:
        return -2;
    }

    // encode
    char cmd[6]{HEADER_PREFIX0, HEADER_PREFIX1, operate, key, 0x00, 0x00};
    cmd[cmd_len - 1] = std::accumulate(cmd, cmd + cmd_len - 1, uint8_t(0));

    // send
    for (int i = 0; i < retry_times_; ++i) {
      auto sendLen = Serial1.write(cmd, cmd_len);
#ifdef DEBUG_IR_COMMAND
      if (logger_ != nullptr) {
        logger_->printf("send command: [%d][%d]", int(operate), sendLen);
        for (auto c : std::string(cmd, sendLen)) {
          logger_->printf("%02X.", uint8_t(c));
        }
        logger_->println();
      }
#endif
      if (sendLen == cmd_len) {
        break;
      }
    }

    // recv
    int expect_len = 0;
    {
      int recv_timeout_ms = 1000;
      int len = 5;
      if (operate == CMD_GET_STATUS) {
        recv_timeout_ms = 20500;  // 学习有20s的超时时间
        len = 12;
      }
      expect_len = len;

      char* buf_end = recv_buf_;
      // for (int i = 0; expect_len && i < retry_times_; ++i) {
      auto recv_len = Serial1.readBytes(buf_end, len);
#ifdef DEBUG_IR_COMMAND
      if (logger_ != nullptr) {
        logger_->printf("read command: [][%d]", recv_len);
        for (auto c : std::string(buf_end, recv_len)) {
          logger_->printf("%02X.", uint8_t(c));
        }
        logger_->println();
      }
#endif
      buf_end += recv_len;
      len -= recv_len;
      // }
      if (len > 0) {
        return -3;
      }

      // check resp
      int checksum = std::accumulate(recv_buf_, recv_buf_ + expect_len - 1, 0);
      checksum &= 0xFF;
      if (checksum != recv_buf_[expect_len - 1]) {
        return -4;
      }
      if (memcmp(cmd, recv_buf_, 3) != 0) {
        return -5;
      }
    }
    return expect_len - 4;  // 去除header、operate和checksum
  }

  int retry_times_;
  Print* logger_;
  char recv_buf_[12];
  String key_names_[MIN_INVALID_KEY];
};

}  // namespace controller
}  // namespace firmware
}  // namespace my

#endif  // MY_ARDUINO_FIRMWARE_IRK02_HPP
