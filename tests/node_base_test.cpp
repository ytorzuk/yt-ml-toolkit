#include <graph/node_base.h>
#include <graph/traversal.h>
#include <array>
#include <list>
#include <memory>
#include <string>
#include <vector>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <iostream>

using namespace yt::graph;
using namespace std::string_literals;

namespace fake_nodes {

class Input : public Node
{
public:
    Input(const std::string &name) : Node{std::move(std::vector<TensorDescriptor::WeakPtr>{}), name}
    {
        outputs_ = {std::make_shared<TensorDescriptor>(yt::DataType::fp32, yt::Shape{12}, this)};
    }
    const OutputsList &inferOutputShapes() override { return outputs_; }
    MOCK_METHOD(void, Die, ());
    ~Input() override { Die(); }
};

class Const : public Node
{
public:
    Const(const std::string &name) : Node{std::move(std::vector<TensorDescriptor::WeakPtr>{}), name}
    {
        outputs_ = {std::make_shared<TensorDescriptor>(yt::DataType::fp32, yt::Shape{12}, this)};
    }
    const OutputsList &inferOutputShapes() override { return outputs_; }
    MOCK_METHOD(void, Die, ());
    ~Const() override { Die(); }
};

class Output : public Node
{
public:
    Output(const TensorDescriptor::WeakPtr &input, const std::string &name) : Node{std::move(std::vector<TensorDescriptor::WeakPtr>{input}), name}
    {
    }
    const OutputsList &inferOutputShapes() override { return outputs_; }
    MOCK_METHOD(void, Die, ());
    ~Output() override { Die(); }
};

class Add : public Node
{
public:
    Add(const TensorDescriptor::WeakPtr &a, const TensorDescriptor::WeakPtr &b, const std::string &name) :
        Node( std::move(std::vector<TensorDescriptor::WeakPtr>{a, b}), name)
    {
        auto aPtr = a.lock();
        outputs_.push_back(std::make_shared<TensorDescriptor>(aPtr->dataType(), aPtr->shape(), this));
    }
    const OutputsList &inferOutputShapes() override { return outputs_; }
    MOCK_METHOD(void, Die, ());
    ~Add() override { Die(); }
};

class Multiply : public Node
{
public:
    explicit Multiply(const TensorDescriptor::WeakPtr &a, const TensorDescriptor::WeakPtr &b, const std::string &name) :
        Node( std::move(std::vector<TensorDescriptor::WeakPtr>{a, b}), name)
    {
        auto aPtr = a.lock();
        outputs_.push_back(std::make_shared<TensorDescriptor>(aPtr->dataType(), aPtr->shape(), this));
    }
    const OutputsList &inferOutputShapes() override { return outputs_; }
    MOCK_METHOD(void, Die, ());
    ~Multiply() override { Die(); }
};

} // namespace fake_nodes


/**
 *
 * Builds a graph
 *
 *   Const   ________________
 *      \   /                \
 *       Add --- Multiply --- Add --- Output
 *      /      /
 *   Input   Const
 *
 */
std::vector<Node::Ptr> buildFakeGraph()
{
    using namespace fake_nodes;
    auto input = std::make_shared<Input>("input_0");
    auto const_0 = std::make_shared<Const>("const_0");
    auto add_0 = std::make_shared<Add>(*input, *const_0, "add_0");
    auto const_1 = std::make_shared<Const>("const_1");
    auto mul_0 = std::make_shared<Multiply>(*add_0, *const_1, "mul_0");
    auto add_1 = std::make_shared<Add>(*mul_0, *add_0, "add_1");
    auto result_0 = std::make_shared<Output>(*add_1, "result_0");
    return {input, const_0, add_0, const_1, mul_0, add_1, result_0};
}


TEST(NodeBaseTest, CheckBFSTraversal)
{
    auto graphNodes = buildFakeGraph();
    std::list<Node*> orderedNodes;
    BFSTraversal(*graphNodes.front(), [&orderedNodes](Node& node) { orderedNodes.push_back(&node); });
    std::string result;
    for (auto &node : orderedNodes)
        result += '/' + node->name();
    EXPECT_EQ(result, "/input_0/add_0/mul_0/add_1/add_1/result_0/result_0");
    using namespace fake_nodes;
    EXPECT_CALL(*std::dynamic_pointer_cast<Input>(graphNodes[0]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Const>(graphNodes[1]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Add>(graphNodes[2]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Const>(graphNodes[3]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Multiply>(graphNodes[4]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Add>(graphNodes[5]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Output>(graphNodes[6]), Die());
}


TEST(NodeBaseTest, CheckBackBFSTraversal)
{
    auto graphNodes = buildFakeGraph();
    std::list<Node*> orderedNodes;
    backBFSTraversal(*graphNodes.back(), [&orderedNodes](Node& node) { orderedNodes.push_back(&node); });
    std::string result;
    for (auto &node : orderedNodes)
        result += '/' + node->name();
    EXPECT_EQ(result, "/result_0/add_1/mul_0/add_0/add_0/const_1/input_0/const_0/input_0/const_0");
    using namespace fake_nodes;
    EXPECT_CALL(*std::dynamic_pointer_cast<Input>(graphNodes[0]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Const>(graphNodes[1]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Add>(graphNodes[2]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Const>(graphNodes[3]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Multiply>(graphNodes[4]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Add>(graphNodes[5]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Output>(graphNodes[6]), Die());
}

