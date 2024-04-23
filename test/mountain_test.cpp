
#include <gtest/gtest.h>
#include <iostream>
#include "src/components/mountain.h"
/*
void checkSectionIndices(Mountain& mountain, float min_x, float max_x){
    IndexInterval interval = mountain.getRelevantMountainSection(min_x, max_x);
    //std::cout << "region: " << mountain.getVertex(interval.start_index).x << " " << mountain.getVertex(interval.end_index).x << " for rock: " << min_x << " " << max_x << std::endl;
    ASSERT_GE(min_x, mountain.getVertex(interval.start_index).x);
    ASSERT_GE(min_x, mountain.getVertex(interval.start_index - 1).x);
    ASSERT_GE(mountain.getVertex(interval.end_index).x, max_x);
    ASSERT_GE(mountain.getVertex(interval.end_index - 1).x, max_x);
    ASSERT_GE(max_x, mountain.getVertex(interval.end_index - 2).x);

    for(std::size_t i = interval.start_index + 1; i < interval.end_index - 1; i++){
        ASSERT_GE(mountain.getVertex(i).x, min_x);
        ASSERT_GE(max_x, mountain.getVertex(i).x);
    }
}

//Test Mountain Initialization and basic relevantMountainSection functionality
TEST(InitializationAndMountainSections, BasicAssertions) {
    Mountain mountain{};
    //some weird errors might occur if there are less than 56 vertices..
    checkSectionIndices(mountain, Mountain::SECTION_WIDTH, Mountain::SECTION_WIDTH*2);
    checkSectionIndices(mountain, Mountain::SECTION_WIDTH*16.2, Mountain::SECTION_WIDTH*24.2);
    checkSectionIndices(mountain, Mountain::SECTION_WIDTH*2.2, Mountain::SECTION_WIDTH*4.0);
    checkSectionIndices(mountain, Mountain::SECTION_WIDTH*45.0, Mountain::SECTION_WIDTH*54.4);
    //std::cout << firstSection.start_index << " " << firstSection.end_index << std::endl;
}

//Test whether loading new chunks breaks the mountain sections
TEST(ChunkLoads, BasicAssertions) {
    Mountain mountain{};
    checkSectionIndices(mountain, Mountain::MOUNTAIN_WIDTH - Mountain::SECTION_WIDTH*10.1, Mountain::MOUNTAIN_WIDTH- Mountain::SECTION_WIDTH*2.1);
    checkSectionIndices(mountain, Mountain::MOUNTAIN_WIDTH - Mountain::SECTION_WIDTH*8.0, Mountain::MOUNTAIN_WIDTH- Mountain::SECTION_WIDTH*2.4);
    mountain.generateNewChunk();
    checkSectionIndices(mountain, Mountain::MOUNTAIN_WIDTH - Mountain::SECTION_WIDTH*2.1, Mountain::MOUNTAIN_WIDTH + Mountain::SECTION_WIDTH*0.0);
    checkSectionIndices(mountain, Mountain::MOUNTAIN_WIDTH - Mountain::SECTION_WIDTH*2.1, Mountain::MOUNTAIN_WIDTH + Mountain::SECTION_WIDTH*3.1);
    mountain.generateNewChunk();
    checkSectionIndices(mountain, Mountain::CHUNK_WIDTH*2 + Mountain::SECTION_WIDTH*1.1, Mountain::CHUNK_WIDTH*2 + Mountain::SECTION_WIDTH*10.2);
    checkSectionIndices(mountain, Mountain::MOUNTAIN_WIDTH - Mountain::SECTION_WIDTH*2.1, Mountain::MOUNTAIN_WIDTH + Mountain::SECTION_WIDTH*3.1);
    mountain.generateNewChunk();
    checkSectionIndices(mountain, Mountain::CHUNK_WIDTH*3 + Mountain::SECTION_WIDTH*3.4, Mountain::CHUNK_WIDTH*3 + Mountain::SECTION_WIDTH*3.9);
    checkSectionIndices(mountain, Mountain::MOUNTAIN_WIDTH + Mountain::CHUNK_WIDTH*2 + Mountain::SECTION_WIDTH*2.8,
                        Mountain::MOUNTAIN_WIDTH + Mountain::CHUNK_WIDTH*2 + Mountain::SECTION_WIDTH*5.4);

    //std::cout << firstSection.start_index << " " << firstSection.end_index << std::endl;
}
*/