#include "mountain.h"
#include "iostream"
#include <random>

// Mountain::Mountain() : random_engine(hardware_random_generator()),
// distribution_used(0.0,1.0) {
Mountain::Mountain() {
    std::cout << "Mountain gets constructed" << std::endl;

    // create points and chunks corresponding to a simple ramp
    double current_x = -MOUNTAIN_WIDTH;
    double current_y = 0.;

    // only initialize a horizontal line
    for (int i = 0; i < landscape_fixpoints_circular_array.size(); i++) {
        landscape_fixpoints_circular_array[i].x = current_x;
        landscape_fixpoints_circular_array[i].y = current_y;
        current_x += SECTION_WIDTH;
    }

    /*
    for (int i = 0; i < landscape_fixpoints_circular_array.size(); i++) {
        landscape_fixpoints_circular_array[i].x = current_x;
        landscape_fixpoints_circular_array[i].y = current_y;
        current_x += SECTION_WIDTH;
        current_y += SECTION_WIDTH * SLOPE;
    }

    for (int i = 0; i < NUMBER_OF_VERTICES / NUM_SECTIONS_PER_CHUNK; i++) {
        generateTerrainRecursive(
            start_of_circular_array,
            start_of_circular_array + NUM_SECTIONS_PER_CHUNK - 1, 50);
        start_of_circular_array =
            (start_of_circular_array + NUM_SECTIONS_PER_CHUNK) %
            NUMBER_OF_VERTICES;

    }*/
    // printTempDebugInfo();
}

void Mountain::printTempDebugInfo() {
    IndexInterval test_interval{getRelevantMountainSection(10.2, 13.6)};
    std::cout << "Mountain test indices: " << test_interval.start_index << ", "
              << test_interval.end_index << std::endl;
    std::cout << "left point coords: " << getVertex(test_interval.start_index).x
              << ", " << getVertex(test_interval.start_index).y << std::endl;
    std::cout << "right point coords: " << getVertex(test_interval.end_index).x
              << ", " << getVertex(test_interval.end_index).y << std::endl;
}

Position Mountain::getVertex(size_t index) {
    return landscape_fixpoints_circular_array[index % NUMBER_OF_VERTICES];
    /*[(index + start_of_circular_array) %
     landscape_fixpoints_circular_array.size()];*/
}

IndexInterval Mountain::getRelevantMountainSection(float_type min_x,
                                                   float_type max_x) {
    IndexInterval returnvalue{};
    returnvalue.start_index =
        ((std::size_t)std::floor(min_x / SECTION_WIDTH)) % NUMBER_OF_VERTICES;
    returnvalue.end_index =
        ((std::size_t)(std::ceil(max_x / SECTION_WIDTH) + 1)) %
        NUMBER_OF_VERTICES;
    if (returnvalue.end_index < returnvalue.start_index)
        returnvalue.end_index += NUMBER_OF_VERTICES;

    return returnvalue;
    /*float leftmost_x = getVertex(0).x;
    IndexInterval returnvalue;
    returnvalue.start_index =
        (std::size_t)std::floor((min_x - leftmost_x) / SECTION_WIDTH);
    returnvalue.end_index =
        (std::size_t)(std::ceil((max_x - leftmost_x) / SECTION_WIDTH) + 1);
    return returnvalue;*/
}

void Mountain::generateNewChunk() {
    std::cout << "Chunk generated" << std::endl;
    generateSlope();
    generateTerrainRecursive(
        start_of_circular_array,
        start_of_circular_array + NUM_SECTIONS_PER_CHUNK - 1, 100);
    start_of_circular_array =
        (start_of_circular_array + NUM_SECTIONS_PER_CHUNK) % NUMBER_OF_VERTICES;
}

void Mountain::generateSlope() {
    int num_points_to_generate = NUM_SECTIONS_PER_CHUNK;
    const std::size_t ARRAY_SIZE = landscape_fixpoints_circular_array.size();
    const std::size_t INDEX_RIGHTEST_VERTICE =
        (start_of_circular_array + ARRAY_SIZE - 1) % ARRAY_SIZE;

    float_type current_x{getVertex(INDEX_RIGHTEST_VERTICE).x};
    float_type current_y{getVertex(INDEX_RIGHTEST_VERTICE).y};
    for (int i = 0; i < num_points_to_generate; i++) {
        current_x += SECTION_WIDTH;
        current_y += SECTION_WIDTH * SLOPE;
        landscape_fixpoints_circular_array[(start_of_circular_array + i) %
                                           ARRAY_SIZE] =
            Position{current_x, current_y};
    }
}

IndexInterval Mountain::getIndexIntervalOfEntireMountain() const {
    IndexInterval returnvalue{};
    returnvalue.start_index = start_of_circular_array;
    returnvalue.end_index = (start_of_circular_array - 1 + NUMBER_OF_VERTICES);
    return returnvalue;
}

IndexInterval Mountain::getLatestChunk() const {
    IndexInterval returnvalue{};
    returnvalue.end_index = start_of_circular_array;
    returnvalue.start_index = (start_of_circular_array -
                               NUM_SECTIONS_PER_CHUNK + NUMBER_OF_VERTICES) %
                              NUMBER_OF_VERTICES;
    if (returnvalue.start_index > returnvalue.end_index) {
        returnvalue.end_index += NUMBER_OF_VERTICES;
    }
    return returnvalue;
}

void Mountain::generateTerrainRecursive(std::size_t leftIndex,
                                        std::size_t rightIndex,
                                        float_type displacement) {
    // if(leftIndex + 1 >= rightIndex){
    //     interpolate(leftIndex, rightIndex);
    //     return;
    // }

    if (leftIndex + 1 == rightIndex)
        return;
    if (leftIndex == rightIndex)
        return;

    if (leftIndex + 32 >= rightIndex)
        displacement = 0;

    std::random_device hardware_random_generator;
    std::mt19937 random_engine(hardware_random_generator());
    std::uniform_real_distribution<float_type> distribution_used(0.0, 1.0);

    std::size_t mid_index = (leftIndex + rightIndex) / 2; // rounding down is
                                                          // fine
    float_type change =
        (distribution_used(random_engine) * 2 - 1) * displacement;
    landscape_fixpoints_circular_array[(mid_index + NUMBER_OF_VERTICES) %
                                       NUMBER_OF_VERTICES]
        .y =
        (landscape_fixpoints_circular_array[(leftIndex + NUMBER_OF_VERTICES) %
                                            NUMBER_OF_VERTICES]
             .y +
         landscape_fixpoints_circular_array[(rightIndex + NUMBER_OF_VERTICES) %
                                            NUMBER_OF_VERTICES]
             .y) /
            2 +
        change;
    displacement = Mountain::ROUGHNESS_TERRAIN * displacement;
    generateTerrainRecursive(leftIndex, mid_index, displacement);
    generateTerrainRecursive(mid_index, rightIndex, displacement);
}

void Mountain::interpolate(std::size_t leftIndex, std::size_t rightIndex) {
    auto left_vert = getVertex(leftIndex);
    auto right_vert = getVertex(rightIndex);
    float_type m = (left_vert.y - right_vert.y) / (right_vert.x - left_vert.x);
    for (int i = 1; i < rightIndex - leftIndex; i++) {
        landscape_fixpoints_circular_array[(leftIndex + i) % NUMBER_OF_VERTICES]
            .y = static_cast<float_type>(i) * m + getVertex(leftIndex).y;
    }
}
