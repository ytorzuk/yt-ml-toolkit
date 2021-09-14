#pragma once

#include "node_base.h"
#include <functional>

namespace yt {
namespace graph {

void BFSTraversal(Node& node, std::function<void(Node&)> callback);
void backBFSTraversal(Node& node, std::function<void(Node&)> callback);

} // graph
} // yt_ml_toolkit
