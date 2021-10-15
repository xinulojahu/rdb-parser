#include <calclib/sum.hpp>

#include <gtest/gtest.h>

TEST(SumTest, Comparison) {
    EXPECT_FLOAT_EQ(calclib::sum_floats(15.31, 23.68), 38.99);
    EXPECT_FLOAT_EQ(calclib::sum_floats(-23.11, 33.10), 9.99);
}
