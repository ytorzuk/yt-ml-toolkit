#pragma once

#include <stdexcept>

namespace yt {

struct Exception : std::runtime_error {
    using std::runtime_error::runtime_error;
};

[[gnu::cold, noreturn]] static inline void throwException(const std::string& msg)
{
    throw Exception{msg};
}

} // namespace exception
