#include "input.h"

namespace yt {
namespace graph {

Input::Input(DataType dtype, Shape shape, const std::string &name) :
    Node {
        std::move(std::vector<TensorDescriptor::WeakPtr>{}),
        name.empty() ? "input_" + std::to_string(genUniqueNameSuffix()) : name
    }
{
    outputs_ = {std::make_shared<TensorDescriptor>(dtype, shape, this)};
}

} // graph
} // yt_ml_toolkit
