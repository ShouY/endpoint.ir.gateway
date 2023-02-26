#ifndef HARDWARD_COMMON_HPP
#define HARDWARD_COMMON_HPP

#include <Arduino.h>

namespace my {
namespace arduino {

// Stream& defaultStream();

void initNetwork(Stream& stream);

}  // namespace arduino
}  // namespace my

#endif  // HARDWARD_COMMON_HPP
