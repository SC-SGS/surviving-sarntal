#include "mountain.h"
#include "iostream"

Mountain::Mountain() {
    std::cout << "Mountain gets constructed" << std::endl;

    // create points and chunks corresponding to a simple ramp
    double current_x = 0.;
    double current_y = 0.;
    for (int i = 0; i < landscape_fixpoints_circular_array.size(); i++) {
        landscape_fixpoints_circular_array[i].x = current_x;
        landscape_fixpoints_circular_array[i].y = current_y;
        current_x += SECTION_WIDTH;
        current_y += SECTION_WIDTH * SLOPE;
    }
    // printTempDebugInfo();
}

void Mountain::printTempDebugInfo() {
    IndexInterval testInterval{getRelevantMountainSection(10.2, 13.6)};
    std::cout << "Mountain test indices: " << testInterval.start_index << ", "
              << testInterval.end_index << std::endl;
    std::cout << "left point coords: " << getVertice(testInterval.start_index).x
              << ", " << getVertice(testInterval.start_index).y << std::endl;
    std::cout << "right point coords: " << getVertice(testInterval.end_index).x
              << ", " << getVertice(testInterval.end_index).y << std::endl;
}

Position Mountain::getVertice(size_t index) {
    return landscape_fixpoints_circular_array[index % NUMBER_OF_VERTICES];
    /*[(index + start_of_circular_array) %
     landscape_fixpoints_circular_array.size()];*/
}

IndexInterval Mountain::getRelevantMountainSection(float min_x, float max_x) {
    IndexInterval returnvalue;
    returnvalue.start_index =
        ((std::size_t)std::floor(min_x / SECTION_WIDTH)) % NUMBER_OF_VERTICES;
    returnvalue.end_index =
        ((std::size_t)(std::ceil(max_x / SECTION_WIDTH) + 1)) %
        NUMBER_OF_VERTICES;
    if (returnvalue.end_index < returnvalue.start_index)
        returnvalue.end_index += NUMBER_OF_VERTICES;

    return returnvalue;
    /*float leftmost_x = getVertice(0).x;
    IndexInterval returnvalue;
    returnvalue.start_index =
        (std::size_t)std::floor((min_x - leftmost_x) / SECTION_WIDTH);
    returnvalue.end_index =
        (std::size_t)(std::ceil((max_x - leftmost_x) / SECTION_WIDTH) + 1);
    return returnvalue;*/
}

void Mountain::generateNewChunk() {
    int num_points_to_generate = NUM_SECTIONS_PER_CHUNK;
    const std::size_t array_size = landscape_fixpoints_circular_array.size();
    const std::size_t index_rightest_vertice =
        (start_of_circular_array + array_size - 1) % array_size;

    float current_x{getVertice(index_rightest_vertice).x};
    float current_y{getVertice(index_rightest_vertice).y};
    for (int i = 0; i < num_points_to_generate; i++) {
        current_x += SECTION_WIDTH;
        current_y += SECTION_WIDTH * SLOPE;
        landscape_fixpoints_circular_array[(start_of_circular_array + i) %
                                           array_size] =
            Position{current_x, current_y};
    }

    start_of_circular_array =
        (start_of_circular_array + num_points_to_generate) % array_size;
}

IndexInterval Mountain::getIndexIntervalOfEntireMountain(){
    IndexInterval returnvalue;
    returnvalue.start_index = start_of_circular_array;
    returnvalue.end_index = (start_of_circular_array - 1 + NUMBER_OF_VERTICES);
}
