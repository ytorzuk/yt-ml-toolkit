#pragma once

#include <vector>
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

class Shape : public std::vector<std::size_t>
{
public:
    using std::vector<std::size_t>::vector;
};

} // yt_ml_toolkit
