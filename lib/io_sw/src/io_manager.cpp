#include "io_manager.hpp"

namespace my {
namespace arduino {
namespace io {

extern Stream& defaultStream() { return Serial; }

}  // namespace io
}  // namespace arduino
}  // namespace my
