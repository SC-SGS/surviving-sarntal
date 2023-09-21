
#include <gtest/gtest.h>
#include <iostream>
#include "src/components/mountain.h"

void checkSectionIndices(Mountain& mountain ,IndexInterval interval, float min_x, float max_x){
    ASSERT_GE(min_x, mountain.getVertice(interval.start_index).x);
    ASSERT_GE(min_x, mountain.getVertice(interval.start_index - 1).x);
    ASSERT_GE(mountain.getVertice(interval.end_index).x, max_x);
    ASSERT_GE(mountain.getVertice(interval.end_index - 1).x, max_x);
    ASSERT_GE(max_x, mountain.getVertice(interval.end_index - 2).x);
}

//Test Mountain Initialization and proper functionality of getMountainSection even in the edgecases
TEST(InitializationAndMountainSections, BasicAssertions) {
    Mountain mountain{};
    float min_x = 0.5;
    float max_x = 1.0;

    checkSectionIndices(mountain, mountain.getRelevantMountainSection(min_x, max_x), min_x, max_x);

    //std::cout << firstSection.start_index << " " << firstSection.end_index << std::endl;
}



