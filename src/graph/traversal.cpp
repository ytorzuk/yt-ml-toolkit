#include "traversal.h"
#include <list>

namespace yt {
namespace graph {

void backBFSTraversal(Node& node, std::function<void(Node&)> callback)
{
    std::list<Node*> list {&node};
    auto current = list.front();
    while (true)
    {
        callback(*current);
        for (const auto &input : current->inputs())
            list.push_back(input.lock()->producer());
        list.pop_front();
        if (list.empty())
            break;
        else
            current = list.front();
    }
}

void BFSTraversal(Node &node, std::function<void (Node &)> callback)
{
    std::list<Node*> list{&node};
    auto current = &node;
    while (true)
    {
        callback(*current);
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


} // namespace yt
} // namespace graph
