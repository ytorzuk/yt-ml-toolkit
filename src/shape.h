#pragma once

#include <array>
#include <memory>

namespace yt {

enum DataType
{
    fp16,
    fp32,
    int8,
    int16,
    int32,
    int64,
    uint8,
    uint16,
    uint32,
    uint64,
};

class Shape : public std::array<std::size_t, 5>
{};

} // yt_ml_toolkit
