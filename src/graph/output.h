#pragma once

#include "node_base.h"

namespace yt::graph {

class Output : public Node
{
public:
    Output(const TensorDescriptor::WeakPtr &input, const std::string &name = std::string{});
};

}
