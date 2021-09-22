#include <graph/output.h>
#include "custom_matchers.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

TEST(InputNodeTest, TestOutputNameAutogeneration)
{
    auto input = std::make_shared<yt::graph::TensorDescriptor>(yt::DataType::int64, yt::Shape{1, 2, 3}, nullptr);
    yt::graph::Output output0 {input};
    yt::graph::Output output1 {input};
    yt::graph::Output output2 {input};
    ASSERT_THAT(output0.name(), ::testing::StartsWith("output_"));
    ASSERT_THAT(output1.name(), ::testing::StartsWith("output_"));
    ASSERT_THAT(output2.name(), ::testing::StartsWith("output_"));
    EXPECT_NE(output0.name(), output1.name());
    EXPECT_NE(output0.name(), output2.name());
    EXPECT_NE(output1.name(), output2.name());
    ASSERT_THAT(output0.name(), EndsWithNumbersAfterUnderscore());
    ASSERT_THAT(output1.name(), EndsWithNumbersAfterUnderscore());
    ASSERT_THAT(output2.name(), EndsWithNumbersAfterUnderscore());
}
