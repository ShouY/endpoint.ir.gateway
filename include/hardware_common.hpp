#ifndef HARDWARD_COMMON_HPP

#include <Arduino.h>

namespace my {

namespace arduino {

Stream& getDefaultStream() { return Serial; }

}  // namespace arduino

}  // namespace my

#endif  // HARDWARD_COMMON_HPP
