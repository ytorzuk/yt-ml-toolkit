#include <throw_exception.h>
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
    BFSTraversal(*graphNodes.front(), [&orderedNodes](Node& node) { orderedNodes.push_back(&node); return true; });
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


TEST(NodeBaseTest, CheckBFSTraversalStop)
{
    auto graphNodes = buildFakeGraph();
    std::list<Node*> orderedNodes;
    BFSTraversal(*graphNodes.front(), [&orderedNodes](Node& node) {
        orderedNodes.push_back(&node);
        return node.name() != "mul_0";
    });
    std::string result;
    for (auto &node : orderedNodes)
        result += '/' + node->name();
    EXPECT_EQ(result, "/input_0/add_0/mul_0");
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
    backBFSTraversal(*graphNodes.back(), [&orderedNodes](Node& node) { orderedNodes.push_back(&node); return true; });
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


TEST(NodeBaseTest, CheckBackBFSTraversalStop)
{
    auto graphNodes = buildFakeGraph();
    std::list<Node*> orderedNodes;
    backBFSTraversal(*graphNodes.back(), [&orderedNodes](Node& node) {
        orderedNodes.push_back(&node);
        return node.name() != "add_0";
    });
    std::string result;
    for (auto &node : orderedNodes)
        result += '/' + node->name();
    EXPECT_EQ(result, "/result_0/add_1/mul_0/add_0");
    using namespace fake_nodes;
    EXPECT_CALL(*std::dynamic_pointer_cast<Input>(graphNodes[0]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Const>(graphNodes[1]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Add>(graphNodes[2]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Const>(graphNodes[3]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Multiply>(graphNodes[4]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Add>(graphNodes[5]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Output>(graphNodes[6]), Die());
}


TEST(NodeBaseTest, CheckDFSTraversal)
{
    using namespace fake_nodes;
    auto graphNodes = buildFakeGraph();
    std::list<Node*> orderedNodes;
    DFSTraversal(*graphNodes.front(), [&orderedNodes](Node& node) { orderedNodes.push_back(&node); return true; });
    std::string result;
    for (auto &node : orderedNodes)
        result += '/' + node->name();
    EXPECT_EQ(result, "/input_0/add_0/mul_0/add_1/result_0/add_1/result_0");
    EXPECT_CALL(*std::dynamic_pointer_cast<Input>(graphNodes[0]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Const>(graphNodes[1]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Add>(graphNodes[2]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Const>(graphNodes[3]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Multiply>(graphNodes[4]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Add>(graphNodes[5]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Output>(graphNodes[6]), Die());
}


TEST(NodeBaseTest, CheckBackDFSTraversal)
{
    using namespace fake_nodes;
    auto graphNodes = buildFakeGraph();
    std::list<Node*> orderedNodes;
    backDFSTraversal(*graphNodes.back(), [&orderedNodes](Node& node) { orderedNodes.push_back(&node); return true; });
    std::string result;
    for (auto &node : orderedNodes)
        result += '/' + node->name();
    EXPECT_EQ(result, "/result_0/add_1/mul_0/add_0/input_0/const_0/const_1/add_0/input_0/const_0");
    EXPECT_CALL(*std::dynamic_pointer_cast<Input>(graphNodes[0]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Const>(graphNodes[1]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Add>(graphNodes[2]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Const>(graphNodes[3]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Multiply>(graphNodes[4]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Add>(graphNodes[5]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Output>(graphNodes[6]), Die());
}


TEST(NodeBaseTest, CheckBackDFSTraversalNoNodeInput)
{
    using namespace fake_nodes;
    auto graphNodes = buildFakeGraph();
    EXPECT_CALL(*std::dynamic_pointer_cast<Input>(graphNodes[0]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Const>(graphNodes[1]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Add>(graphNodes[2]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Const>(graphNodes[3]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Multiply>(graphNodes[4]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Add>(graphNodes[5]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Output>(graphNodes[6]), Die());
    graphNodes.erase(graphNodes.begin());
    std::list<Node*> orderedNodes;
    EXPECT_THROW(
                backDFSTraversal(*graphNodes.back(), [&orderedNodes](Node& node) {
                    orderedNodes.push_back(&node);
                    return true;
                }), yt::Exception);
}


TEST(NodeBaseTest, CheckBackBFSTraversalNoNodeInput)
{
    using namespace fake_nodes;
    auto graphNodes = buildFakeGraph();
    EXPECT_CALL(*std::dynamic_pointer_cast<Input>(graphNodes[0]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Const>(graphNodes[1]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Add>(graphNodes[2]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Const>(graphNodes[3]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Multiply>(graphNodes[4]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Add>(graphNodes[5]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Output>(graphNodes[6]), Die());
    graphNodes.erase(graphNodes.begin());
    std::list<Node*> orderedNodes;
    EXPECT_THROW(
                backBFSTraversal(*graphNodes.back(), [&orderedNodes](Node& node) {
                    orderedNodes.push_back(&node);
                    return true;
                }), yt::Exception);
}



TEST(NodeBaseTest, CheckTraverseInExecutionOrder)
{
    using namespace fake_nodes;
    auto graphNodes = buildFakeGraph();
    EXPECT_CALL(*std::dynamic_pointer_cast<Input>(graphNodes[0]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Const>(graphNodes[1]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Add>(graphNodes[2]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Const>(graphNodes[3]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Multiply>(graphNodes[4]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Add>(graphNodes[5]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Output>(graphNodes[6]), Die());
    std::list<Node*> orderedNodes;
    std::string result;
    traverseInExecutionOrder({graphNodes.front()}, {graphNodes.back()}, [&result](Node::Ptr node) { result += '/' + node->name(); });
    EXPECT_EQ(result, "/const_0/input_0/add_0/const_1/mul_0/add_1/result_0");
}


/**
 *
 * Builds a graph
 *
 *   Const   ________________
 *      \   /                \
 *       Add --- Multiply --- Add --- Output
 *      /      /         \   /
 *   Input   Const        Add ------- Output
 *
 */
std::vector<Node::Ptr> buildFakeGraphWith2Outputs()
{
    using namespace fake_nodes;
    auto input = std::make_shared<Input>("input_0");
    auto const_0 = std::make_shared<Const>("const_0");
    auto add_0 = std::make_shared<Add>(*input, *const_0, "add_0");
    auto const_1 = std::make_shared<Const>("const_1");
    auto mul_0 = std::make_shared<Multiply>(*add_0, *const_1, "mul_0");
    auto add_1 = std::make_shared<Add>(*mul_0, *add_0, "add_1");
    auto add_2 = std::make_shared<Add>(*mul_0, *add_1, "add_2");
    auto result_0 = std::make_shared<Output>(*add_1, "result_0");
    auto result_1 = std::make_shared<Output>(*add_2, "result_1");
    return {input, const_0, add_0, const_1, mul_0, add_1, add_2, result_0, result_1};
}


TEST(NodeBaseTest, CheckTraverseInExecutionOrder2Outputs)
{
    using namespace fake_nodes;
    auto graphNodes = buildFakeGraphWith2Outputs();
    EXPECT_CALL(*std::dynamic_pointer_cast<Input>(graphNodes[0]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Const>(graphNodes[1]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Add>(graphNodes[2]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Const>(graphNodes[3]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Multiply>(graphNodes[4]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Add>(graphNodes[5]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Add>(graphNodes[6]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Output>(graphNodes[7]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Output>(graphNodes[8]), Die());
    std::list<Node*> orderedNodes;
    std::string result;
    traverseInExecutionOrder({graphNodes.front()}, {*(graphNodes.end() - 2), graphNodes.back()},
                             [&result](Node::Ptr node) { result += '/' + node->name(); });
    EXPECT_EQ(result, "/const_0/input_0/add_0/const_1/mul_0/add_1/result_0/add_2/result_1");
}


TEST(NodeBaseTest, CheckTraverseInExecutionOrderDisconnectedInput)
{
    using namespace fake_nodes;
    auto graphNodes = buildFakeGraphWith2Outputs();
    auto disconnectedInput = std::make_shared<Input>("input_0");
    EXPECT_CALL(*std::dynamic_pointer_cast<Input>(disconnectedInput), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Input>(graphNodes[0]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Const>(graphNodes[1]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Add>(graphNodes[2]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Const>(graphNodes[3]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Multiply>(graphNodes[4]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Add>(graphNodes[5]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Add>(graphNodes[6]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Output>(graphNodes[7]), Die());
    EXPECT_CALL(*std::dynamic_pointer_cast<Output>(graphNodes[8]), Die());
    std::list<Node*> orderedNodes;
    std::string result;
    EXPECT_THROW(traverseInExecutionOrder({disconnectedInput}, {*(graphNodes.end() - 2), graphNodes.back()}),
                 yt::Exception);
}
