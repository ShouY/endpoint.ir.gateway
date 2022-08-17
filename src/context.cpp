#include "context.hpp"

#include <asio.hpp>

namespace my {

Terminal* Context::this_terminal() {
  if (term_stack_.empty()) {
    return nullptr;
  }
  return term_stack_.back();
}

int Context::exec(Terminal* term) {
  if (term == nullptr) return -1;
  term_stack_.push_back(term);
  return 0;
}

void Context::run() {
  if (term_stack_.empty()) return;

  term_stack_.back()->poll();
  for (auto begin = term_stack_.rbegin(); begin != term_stack_.rend();
       begin++) {
    auto* term = *begin;
    term->loop();
  }
}

Terminal::~Terminal() { this->exit(); }

void Terminal::run() {
  poll();
  loop();
}

void Terminal::poll() {}

void Terminal::loop() {}

}  // namespace my
