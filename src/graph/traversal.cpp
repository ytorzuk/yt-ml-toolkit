#include <throw_exception.h>
#include "traversal.h"
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


} // namespace yt
} // namespace graph
