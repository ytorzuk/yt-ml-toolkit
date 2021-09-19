#include <throw_exception.h>
#include <graph/output.h>
#include "traversal.h"
#include <algorithm>
#include <list>
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

Nodes traverseInExecutionOrder(const Nodes &nodes, std::function<void (Node::Ptr)> callback, const Nodes &outputs)
{
    Nodes outputNodes {};
    if (outputs.empty())
        std::copy_if(nodes.begin(), nodes.end(), outputNodes.begin(), [](Node::Ptr node) { return std::dynamic_pointer_cast<graph::Output>(node); });
    else
        std::reverse_copy(outputs.begin(), outputs.end(), std::back_inserter(outputNodes));
    Nodes orderedNodes {};
    for (auto& output : outputNodes)
        backDFSTraversal(*output, [&orderedNodes](Node& node) { orderedNodes.push_back(node.shared_from_this()); return true; });
    std::reverse(orderedNodes.begin(), orderedNodes.end());
    auto last = orderedNodes.end();
    for (auto it = orderedNodes.begin(); it != last; it++)
        last = std::remove(it + 1, last, *it);
    orderedNodes.erase(last, orderedNodes.end());
    if (callback)
        for (auto& node : orderedNodes)
            callback(node);
    return orderedNodes;
}


} // namespace yt
} // namespace graph
