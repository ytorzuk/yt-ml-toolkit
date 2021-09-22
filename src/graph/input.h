#pragma once

#include "node_base.h"

namespace yt {
namespace graph {

class Input : public Node
{
public:
    Input(DataType dtype, Shape shape, const std::string &name = std::string{});
};

} // graph
} // yt_ml_toolkit

