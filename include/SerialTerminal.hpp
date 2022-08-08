#pragma once

#define ST_VERSION "1.1.2"

#include <Arduino.h>

namespace maschinendeck {

#if not defined ST_FLAG_NOBUILTIN && defined E2END
#include "EEPROM.h"
void printEEPROM(String opts) {
  io_stream->print("offset \t");
  for (uint8_t h = 0; h < 16; h++) {
    io_stream->print(h, HEX);
    io_stream->print('\t');
  }
  io_stream->print("\r\n");
  for (uint8_t i = 0; i < (E2END / 16); i++) {
    String line = "";
    io_stream->print(i * 16, HEX);
    io_stream->print('\t');
    for (uint8_t n = 0; n <= 15; n++) {
      size_t value = EEPROM.read(i * 16 + n);
      io_stream->print(value, HEX);
      io_stream->print('\t');
      line += static_cast<char>(value);
    }
    io_stream->print(line);
    io_stream->print("\r\n");
  }
}
#endif

struct Command {
  String command;
  void (*callback)(String param);
  String description;

  Command(String command, void (*callback)(String param), String description)
      : command(command), callback(callback), description(description) {}
};

template <typename T, typename U>
class Pair {
 private:
  T first_;
  U second_;

 public:
  Pair(T first, U second) : first_(first), second_(second) {}
  T first() { return this->first_; }

  U second() { return this->second_; }
};

enum CoroutineStatus {
  iyield = 0,
  iawait = 1,
  ireturn = 2,
};

class SerialTerminal {
 private:
  Stream* io_stream;
  Command* commands[256];
  uint8_t size_;
  bool firstRun;
  String message;

#ifndef ST_FLAG_NOHELP
  void printCommands() {
    for (uint8_t i = 0; i < this->size_; i++) {
      io_stream->println("\t" + this->commands[i]->command + "\t" +
                         this->commands[i]->description);
    }
#ifndef ST_FLAG_NOPROMPT
    io_stream->print("st> ");
#endif
  }
#endif

 public:
  SerialTerminal(Stream* io = nullptr)
      : io_stream(io), size_(0), firstRun(true), message("") {
#if not defined ST_FLAG_NOBUILTIN && defined E2END
    this->add("eeprom", &printEEPROM, "prints the contents of EEPROM");
#endif

#ifndef ST_FLAG_NOHELP
    if (io_stream) {
      io_stream->print("SerialTerm v" ST_VERSION);
      io_stream->print("\r\n");
      io_stream->println("(C) 2022, MikO - Hpsaturn");
      io_stream->println("  available commands:");
    }
#endif
  }

  void init(Stream* io) { io_stream = io; }

  bool is_open() const { return io_stream && io_stream->availableForWrite(); }

  void add(String command, void (*callback)(String param),
           String description = "") {
    if (this->size_ >= 64) return;
    this->commands[this->size_] = new Command(command, callback, description);
    this->size_++;
  }

  uint8_t size() { return this->size_ + 1; }

  void loop() {
    if (io_stream == nullptr) {
      return;
    }
#ifndef ST_FLAG_NOHELP
    if (this->firstRun) {
      this->firstRun = false;
      this->printCommands();
    }
#endif
    if (!io_stream->available()) return;
    bool commandComplete = false;
    static CoroutineStatus next_action = CoroutineStatus::iyield;
    while (io_stream->available()) {
      char car = io_stream->read();
      switch (car) {
        case char(8):
        case char(127):
          if (this->message.length() > 0) {
            io_stream->print("\b \b");
            this->message.remove(this->message.length() - 1);
          }
          next_action = CoroutineStatus::iawait;
          break;
        case '\r':
          io_stream->print("\r\n");
          commandComplete = true;
          // If there are more data on the line, drop a \n, if it is
          // there. Some terminals may send both, giving
          // an extra lineend, if we do not drop it.
          if (io_stream->available() && io_stream->peek() == '\n') {
            io_stream->read();
          }
          io_stream->flush();
          io_stream->read();
          next_action = CoroutineStatus::ireturn;
          break;
        default:
          if (isAscii(car)) io_stream->print(car);
          next_action = CoroutineStatus::iyield;
      }
      if (next_action == CoroutineStatus::iyield) {
        this->message += car;
      } else if (next_action == CoroutineStatus::iawait) {
        continue;
      } else if (next_action == CoroutineStatus::ireturn) {
        break;
      }
    }

    if (!commandComplete) return;
    if (this->message == "") {
#ifndef ST_FLAG_NOPROMPT
      io_stream->print("st> ");
#endif

      return;
    }

    Pair<String, String> command = SerialTerminal::ParseCommand(this->message);
    this->message = "";
    bool found = false;
    for (uint8_t i = 0; i < this->size_; i++) {
      if (this->commands[i]->command == command.first()) {
        this->commands[i]->callback(command.second());
        found = true;
      }
    }
    if (!found) {
      io_stream->print("\n" + command.first());
      io_stream->println(": command not found");
    }
#ifndef ST_FLAG_NOPROMPT
    io_stream->print("\r\nst> ");
#endif
  }

  static Pair<String, String> ParseCommand(String message) {
    String keyword = "";
    for (auto& car : message) {
      if (car == ' ') break;
      keyword += car;
    }
    if (keyword != "") message.remove(0, keyword.length());
    keyword.trim();
    message.trim();

    return Pair<String, String>(keyword, message);
  }

  static String ParseArgument(String message) {
    message.trim();
    return message;
  }
};

}  // namespace maschinendeck
