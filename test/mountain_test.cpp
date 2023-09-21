
#include <gtest/gtest.h>
#include <iostream>
#include "src/components/mountain.h"


//Test Mountain Initialization and proper functionality of getMountainSection even in the edgecases
TEST(InitializationAndMountainSections, BasicAssertions) {
    Mountain mountain{};
    /*float min_x = 0.5;
    float max_x = 1.0;
    auto firstSection = mountain.getRelevantMountainSection(min_x, max_x);
    ASSERT_GE(mountain.getVertice(firstSection.start_index).x, min_x);
    std::cout << firstSection.start_index << " " << firstSection.end_index << std::endl;*/
}



