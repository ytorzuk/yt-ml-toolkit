#include <graph/input.h>
#include "custom_matchers.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

TEST(InputNodeTest, TestOutputShapeInference)
{
    yt::graph::Input input {yt::DataType::int64, {1, 2, 3}};
    EXPECT_EQ(input.outputs()[0]->dataType(), yt::DataType::int64);
    ASSERT_THAT(input.outputs()[0]->shape(), ::testing::ElementsAre(1, 2, 3));
}


TEST(InputNodeTest, TestInputName)
{
    yt::graph::Input input {yt::DataType::int64, {1, 2, 3}, "input_321"};
    EXPECT_EQ(input.name(), "input_321");
}


TEST(InputNodeTest, TestInputNameAutogeneration)
{
    yt::graph::Input input0 {yt::DataType::int64, {1, 2, 3}};
    yt::graph::Input input1 {yt::DataType::int64, {1, 2, 3}};
    yt::graph::Input input2 {yt::DataType::int64, {1, 2, 3}};
    ASSERT_THAT(input0.name(), ::testing::StartsWith("input_"));
    ASSERT_THAT(input1.name(), ::testing::StartsWith("input_"));
    ASSERT_THAT(input2.name(), ::testing::StartsWith("input_"));
    EXPECT_NE(input0.name(), input1.name());
    EXPECT_NE(input0.name(), input2.name());
    EXPECT_NE(input1.name(), input2.name());
    ASSERT_THAT(input0.name(), EndsWithNumbersAfterUnderscore());
    ASSERT_THAT(input1.name(), EndsWithNumbersAfterUnderscore());
    ASSERT_THAT(input2.name(), EndsWithNumbersAfterUnderscore());
}
