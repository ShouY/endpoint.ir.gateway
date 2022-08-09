#ifndef TERMINAL_CONTEXT_HPP
#define TERMINAL_CONTEXT_HPP

#include <list>
#include <string>

namespace my {

class Terminal;

class Context {
 public:
  Terminal* this_terminal();
  int exec(Terminal* term);
  void run();

 private:
  std::list<Terminal*> term_stack_;
};

class Terminal {
 public:
  Terminal(Context& ctx);
  virtual ~Terminal();

  inline virtual void run();
  virtual void exit() final;

  friend class Context;

 protected:
  virtual void poll();
  virtual void loop();

 private:
  Context& ctx;
};

}  // namespace my

#endif TERMINAL_CONTEXT_HPP
