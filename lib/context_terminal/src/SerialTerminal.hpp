#pragma once

#ifndef MY_CONTEXT_TERMINAL_HPP
#define MY_CONTEXT_TERMINAL_HPP

#define ST_VERSION "1.1.2"

#include <Arduino.h>

#define MAX_TERMINAL_COMMAND_SIZE 256

namespace maschinendeck {

#if not defined ST_FLAG_NOBUILTIN && defined E2END
#include "EEPROM.h"
void printEEPROM(String opts) {
  io->print("offset \t");
  for (uint8_t h = 0; h < 16; h++) {
    io->print(h, HEX);
    io->print('\t');
  }
  io->print("\r\n");
  for (uint8_t i = 0; i < (E2END / 16); i++) {
    String line = "";
    io->print(i * 16, HEX);
    io->print('\t');
    for (uint8_t n = 0; n <= 15; n++) {
      size_t value = EEPROM.read(i * 16 + n);
      io->print(value, HEX);
      io->print('\t');
      line += static_cast<char>(value);
    }
    io->print(line);
    io->print("\r\n");
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

enum CoroutineStatus {
  iyield = 0,
  iawait = 1,
  ireturn = 2,
};

class SerialTerminal {
 private:
  Stream* io;
  Command* commands[MAX_TERMINAL_COMMAND_SIZE];
  uint8_t size_;
  bool firstRun;
  String message;

#ifndef ST_FLAG_NOHELP
  void printCommands() {
    for (uint8_t i = 0; i < this->size_; i++) {
      io->println("\t" + this->commands[i]->command + "\t" +
                         this->commands[i]->description);
    }
#ifndef ST_FLAG_NOPROMPT
    io->print("st> ");
#endif
  }
#endif

 public:
  SerialTerminal(Stream* io = nullptr)
      : io(io), size_(0), firstRun(true), message("") {
#if not defined ST_FLAG_NOBUILTIN && defined E2END
    this->add("eeprom", &printEEPROM, "prints the contents of EEPROM");
#endif

    // #ifndef ST_FLAG_NOHELP
    //     if (io) {
    //       io->print("SerialTerm v" ST_VERSION);
    //       io->print("\r\n");
    //       io->println("(C) 2022, MikO - Hpsaturn");
    //       io->println("  available commands:");
    //     }
    // #endif
  }

  void init(Stream* io) { io = io; }

  bool is_open() const { return io && io->availableForWrite(); }

  // Add command to terminal.
  //
  // return 0 if add success, -1 when command size is full, -2 is command is
  // existed.
  int add(String command, void (*callback)(String param),
          String description = "") {
    if (this->size_ >= MAX_TERMINAL_COMMAND_SIZE) return -1;
    for (int i = 0; i < this->size_; ++i) {
      if (this->commands[i]->command == command) {
        return -2;
      }
    }
    this->commands[this->size_] = new Command(command, callback, description);
    this->size_++;
    return 0;
  }

  uint8_t size() { return this->size_ + 1; }

  void loop() {
    if (io == nullptr) {
      return;
    }
#ifndef ST_FLAG_NOHELP
    if (this->firstRun) {
      this->firstRun = false;
      this->printCommands();
    }
#endif
    if (!io->available()) return;
    bool commandComplete = false;
    static CoroutineStatus next_action = CoroutineStatus::iyield;
    while (io->available()) {
      char car = io->read();
      switch (car) {
        case char(8):
        case char(127):
          if (this->message.length() > 0) {
            io->print("\b \b");
            this->message.remove(this->message.length() - 1);
          }
          next_action = CoroutineStatus::iawait;
          break;
        case '\r':
          io->print("\r\n");
          commandComplete = true;
          // If there are more data on the line, drop a \n, if it is
          // there. Some terminals may send both, giving
          // an extra lineend, if we do not drop it.
          if (io->available() && io->peek() == '\n') {
            io->read();
          }
          io->flush();
          io->read();
          next_action = CoroutineStatus::ireturn;
          break;
        default:
          if (isAscii(car)) io->print(car);
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
      io->print("st> ");
#endif

      return;
    }

    std::pair<String, String> command =
        SerialTerminal::ParseCommand(this->message);
    this->message = "";
    bool found = false;
    for (uint8_t i = 0; i < this->size_; i++) {
      if (this->commands[i]->command == command.first) {
        this->commands[i]->callback(command.second);
        found = true;
      }
    }
    if (!found) {
      io->print("\n" + command.first);
      io->println(": command not found");
    }
#ifndef ST_FLAG_NOPROMPT
    io->print("\r\nst> ");
#endif
  }

  static std::pair<String, String> ParseCommand(String message) {
    String keyword = "";
    for (auto& car : message) {
      if (car == ' ') break;
      keyword += car;
    }
    if (keyword != "") message.remove(0, keyword.length());
    keyword.trim();
    message.trim();

    return std::pair<String, String>(keyword, message);
  }

  static String ParseArgument(String message) {
    message.trim();
    return message;
  }
};

}  // namespace maschinendeck

#endif  // MY_CONTEXT_TERMINAL_HPP
