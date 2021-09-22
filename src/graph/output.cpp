#include "output.h"

namespace yt::graph {

Output::Output(const TensorDescriptor::WeakPtr &input, const std::string &name) :
    Node{ std::move(std::vector<TensorDescriptor::WeakPtr>{input}),
        name.empty() ? "output_" + std::to_string(genUniqueNameSuffix()) : name }
{

}

}
