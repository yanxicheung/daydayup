#include "gtest/gtest.h"
#include "Length.h"
struct LengthTest: testing::Test
{

};

TEST_F(LengthTest,1_FEET_should_equal_to_12_INCH)
{
    ASSERT_TRUE(Length(1, FEET) == Length(12, INCH));
}
