#include <throw_exception.h>
#include <graph/output.h>
#include "traversal.h"
#include <algorithm>
#include <list>
#include <set>
#include <string>

namespace yt {
namespace graph {

using namespace std::string_literals;

void backBFSTraversal(Node& node, std::function<bool(Node&)> callback)
{
    std::list<Node*> list {&node};
    auto current = list.front();
    while (callback(*current))
    {
        auto &inputs = current->inputs();
        auto numInputs = inputs.size();
        for (int i = 0; i < numInputs; i++)
        {
            auto input = inputs[i].lock();
            if (input)
                list.push_back(input->producer());
            else
                throwException("backBFSTraversal Failure: Input #"s + std::to_string(i) + " of "s + current->name() + " is not available"s);
        }
        list.pop_front();
        if (list.empty())
            break;
        else
            current = list.front();
    }
}


void BFSTraversal(Node &node, std::function<bool(Node &)> callback)
{
    std::list<Node*> list{&node};
    auto current = &node;
    while (callback(*current))
    {
        for (const auto &output : current->outputs())
            for (auto const& consumer : output->consumers())
                list.push_back(consumer);
        list.pop_front();
        if (list.empty())
            break;
        else
            current = list.front();
    }
}


void DFSTraversal(Node &node, std::function<bool (Node &)> callback)
{
    if (!callback(node))
        return;
    for (auto& output : node.outputs())
        for (auto& consumer : output->consumers())
            DFSTraversal(*consumer, callback);
}


void backDFSTraversal(Node &node, std::function<bool (Node &)> callback)
{
    if (!callback(node))
        return;
    auto &inputs = node.inputs();
    auto numInputs = inputs.size();
    for (int i = 0; i < numInputs; i++)
    {
        auto input = inputs[i].lock();
        if (input)
            backDFSTraversal(*input->producer(), callback);
        else
            throwException("backDFSTraversal Failure: Input #"s + std::to_string(i) + " of "s + node.name() + " is not available"s);
    }
}

Nodes traverseInExecutionOrder(const Nodes &inputs, const Nodes &outputs, std::function<void(Node::Ptr)> callback)
{
    Nodes reverseOutputs {outputs.rbegin(), outputs.rend()};
    Nodes orderedNodes {};
    for (auto& output : reverseOutputs)
        backDFSTraversal(*output, [&orderedNodes](Node& node) { orderedNodes.push_back(node.shared_from_this()); return true; });
    std::reverse(orderedNodes.begin(), orderedNodes.end());
    auto last = orderedNodes.end();
    for (auto it = orderedNodes.begin(); it != last; it++)
        last = std::remove(it + 1, last, *it);
    orderedNodes.erase(last, orderedNodes.end());
    std::set<Node::Ptr> nodesSet {orderedNodes.begin(), orderedNodes.end()};
    for (auto& input : inputs)
        if (nodesSet.find(input) == nodesSet.end())
            throwException("Input "s + input->name() + " is not part of the graph"s);
    if (callback)
        for (auto& node : orderedNodes)
            callback(node);
    return orderedNodes;
}


} // namespace yt
} // namespace graph
