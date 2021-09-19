#pragma once

#include "shape.h"
#include <functional>
#include <string>
#include <vector>

namespace yt {
namespace graph {

class Node;

class TensorDescriptor
{
public:
    using Ptr = std::shared_ptr<TensorDescriptor>;
    using WeakPtr = std::weak_ptr<TensorDescriptor>;
    using NodesList = std::vector<Node*>;

    TensorDescriptor(DataType dtype, Shape shape, Node* producer);
    ~TensorDescriptor();
    DataType dataType() const;
    const Shape &shape() const;
    NodesList &consumers();
    const NodesList &consumers() const;
    const Node* producer() const;
    Node *producer();

private:
    DataType dtype_;
    Shape shape_;
    Node* producer_;
    NodesList consumers_;
};

class Node : public std::enable_shared_from_this<Node>
{
public:
    using OutputsList = std::vector<TensorDescriptor::Ptr>;
    using InputsList = std::vector<TensorDescriptor::WeakPtr>;
    using Ptr = std::shared_ptr<Node>;

    explicit Node(const std::vector<TensorDescriptor::WeakPtr> &inputs, const std::string &name);
    explicit Node(std::vector<TensorDescriptor::WeakPtr> &&inputs, const std::string &name);
    virtual ~Node();
    const std::string &name() const;
    virtual const OutputsList &inferOutputShapes() = 0;
    OutputsList& outputs();
    const OutputsList &outputs() const;
    InputsList& inputs();
    const InputsList &inputs() const;
    virtual operator TensorDescriptor::Ptr();
    virtual operator TensorDescriptor::WeakPtr();

protected:
    std::string name_;
    InputsList inputs_;
    // To be initialized in a subclass
    OutputsList outputs_;
};

} // graph
} // yt_ml_toolkit
