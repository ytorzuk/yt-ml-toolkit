#pragma once

#include "node_base.h"
#include <functional>
#include <vector>

namespace yt {
namespace graph {

using Nodes = std::vector<Node::Ptr>;

Nodes traverseInExecutionOrder(const Nodes &inputs, const Nodes &outputs, std::function<void(Node::Ptr)> callback = nullptr);
void BFSTraversal(Node& node, std::function<bool(Node&)> callback);
void backBFSTraversal(Node& node, std::function<bool(Node&)> callback);
void DFSTraversal(Node& node, std::function<bool(Node&)> callback);
void backDFSTraversal(Node& node, std::function<bool(Node&)> callback);

} // graph
} // yt_ml_toolkit
