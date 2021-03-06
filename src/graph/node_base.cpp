#include "node_base.h"

namespace yt {
namespace graph {

unsigned Node::uniqueNameSuffix {};

Node::Node(const std::vector<TensorDescriptor::WeakPtr> &inputs, const std::string &name) :
    name_ {name},
    inputs_ {inputs.begin(), inputs.end()}
{
    for (auto &input : inputs_)
        if (!input.expired())
            input.lock()->consumers().push_back(this);
}

Node::Node(std::vector<TensorDescriptor::WeakPtr> &&inputs, const std::string &name) :
    name_ {name},
    inputs_ {std::move(inputs)}
{
    for (auto &input : inputs_)
        if (!input.expired())
            input.lock()->consumers().push_back(this);
}

Node::~Node()
{
    for (auto input : inputs_)
    {
        if (!input.expired())
        {
            auto &consumers = input.lock()->consumers();
            consumers.erase(std::remove_if(consumers.begin(), consumers.end(), [this](Node* node) { return node == this; }),
                    consumers.end());
        }
    }
}

const std::string &Node::name() const
{
    return name_;
}

Node::OutputsList &Node::outputs()
{
    return outputs_;
}

const Node::OutputsList &Node::outputs() const
{
    return outputs_;
}

Node::InputsList &Node::inputs()
{
    return inputs_;
}

const Node::InputsList &Node::inputs() const
{
    return inputs_;
}

Node::operator TensorDescriptor::WeakPtr()
{
    return outputs_.front();
}

Node::operator TensorDescriptor::Ptr()
{
    return outputs_.front();
}

TensorDescriptor::TensorDescriptor(DataType dtype, Shape shape, Node *producer) :
    dtype_ {dtype},
    shape_ {shape},
    producer_ {producer}
{
}

TensorDescriptor::~TensorDescriptor()
{
    for (auto consumer : consumers_)
    {
        auto &inputsList = consumer->inputs();
        inputsList.erase(std::remove_if(inputsList.begin(),
                                        inputsList.end(),
                                        [this](TensorDescriptor::WeakPtr ptr){ return ptr.lock().get() == this; }),
                inputsList.end());
    }
}

DataType TensorDescriptor::dataType() const
{
    return dtype_;
}

const Shape &TensorDescriptor::shape() const
{
    return shape_;
}

TensorDescriptor::NodesList &TensorDescriptor::consumers()
{
    return consumers_;
}

const TensorDescriptor::NodesList &TensorDescriptor::consumers() const
{
    return consumers_;
}

const Node* TensorDescriptor::producer() const
{
    return producer_;
}

Node *TensorDescriptor::producer()
{
    return producer_;
}

}
}
