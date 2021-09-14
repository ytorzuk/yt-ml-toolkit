#include "traversal.h"
#include <list>

namespace yt {
namespace graph {

void backBFSTraversal(Node& node, std::function<bool(Node&)> callback)
{
    std::list<Node*> list {&node};
    auto current = list.front();
    while (callback(*current))
    {
        for (const auto &input : current->inputs())
            list.push_back(input.lock()->producer());
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


} // namespace yt
} // namespace graph
