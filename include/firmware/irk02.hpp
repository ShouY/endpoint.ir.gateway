/* IR Remote Controller IRK02
 *
 * Document:
 * https://item.taobao.com/item.htm?spm=a1z09.2.0.0.67072e8dm58dp9&id=663016557026&_u=m2ep4l0c71f6
 *
 */

#include <Arduino.h>

#include <numeric>
#include <string>

namespace my {
namespace arduino {

class IRControllor {
  static constexpr char HEADER0 = 0x55;
  static constexpr char HEADER1 = 0xAA;

  static constexpr char CMD_STATUS = 0x01;
  static constexpr char CMD_LEARN = 0x02;
  static constexpr char CMD_EMIT = 0x03;
  static constexpr char CMD_BAUD = 0x04;
  static constexpr char CMD_INVALID = 0x04;

  static constexpr char KEY_INVALID = 48;

  static constexpr char BAUD002400 = 0x00;
  static constexpr char BAUD004800 = 0x01;
  static constexpr char BAUD009600 = 0x02;
  static constexpr char BAUD019200 = 0x03;
  static constexpr char BAUD115200 = 0x04;
  static constexpr char BAUD_invalid = 0x05;

  // 响应信息在返回报文中的起始位置偏移
  static constexpr uint8_t RESPONSE_OFFSET = 3;
  static constexpr char CMD_SUCCESS = 0x01;

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
  bool status(int key) {
    if (key >= KEY_INVALID) {
      return false;
    }
    auto respLen = sendCmd(CMD_STATUS, 0x00);
    Serial.printf("status resp len=%d\n", respLen);
    if (respLen != 8) {
      return false;
    }
    auto s = recv_buf_[RESPONSE_OFFSET + 8 - key / 8];
    key %= 8;
    return bitRead(s, key);
  }

 private:
  bool validCode(uint8_t code) const noexcept { return code < 48; }

  int sendCmd(char operate, char key) {
    size_t cmd_len;  // 命令的长度
    // check
    switch (operate) {
      case CMD_LEARN:
      case CMD_EMIT:
        if (uint8_t(key) >= KEY_INVALID) {
          return -1;
        }
        cmd_len = 5;
        break;
      case CMD_STATUS:
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
    char cmd[6]{HEADER0, HEADER1, operate, key, 0x00, 0x00};
    cmd[cmd_len - 1] = std::accumulate(cmd, cmd + cmd_len - 1, uint8_t(0));

    // send
    Serial1.setTimeout(200);
    for (int i = 0; i < retry_times_; ++i) {
      auto sendLen = Serial1.write(cmd, cmd_len);
      if (sendLen == cmd_len) {
        break;
      }
    }

    // recv
    int expect_len = 0;
    {
      int recv_timeout_ms = 1000;
      int len = 5;
      if (operate == CMD_STATUS) {
        recv_timeout_ms = 20500;  // 学习有20s的超时时间
        len = 12;
      }
      expect_len = len;

      char* buf_end = recv_buf_;
      // for (int i = 0; expect_len && i < retry_times_; ++i) {
      auto recv_len = Serial1.readBytes(buf_end, len);
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
  String key_names_[KEY_INVALID];
};

}  // namespace arduino
}  // namespace my
