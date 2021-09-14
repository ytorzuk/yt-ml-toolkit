#pragma once

#include "node_base.h"
#include <functional>

namespace yt {
namespace graph {

void BFSTraversal(Node& node, std::function<bool(Node&)> callback);
void backBFSTraversal(Node& node, std::function<bool(Node&)> callback);
void DFSTraversal(Node& node, std::function<bool(Node&)> callback);

} // graph
} // yt_ml_toolkit
