#include <gtest/gtest.h>
#include "Utils/ArrayUtils.h"

using namespace std;

TEST(ArrayFormatting, HandlesEmptyVector) 
{
    std::vector<int> empty_vec;
    EXPECT_EQ(Utils::formatArrayToString(empty_vec), "[ ]");
}

TEST(ArrayFormatting, HandlesSingleElementVector) 
{
    std::vector<int> single_vec = {42};
    EXPECT_EQ(Utils::formatArrayToString(single_vec), "[ 42 ]");
}

TEST(ArrayFormatting, HandlesMultipleElementsVector) 
{
    std::vector<int> multi_vec = {1, 0, 5, 9};
    EXPECT_EQ(Utils::formatArrayToString(multi_vec), "[ 1 0 5 9 ]");
}