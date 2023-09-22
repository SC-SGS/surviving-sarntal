#include "mountain.h"
#include "iostream"
#include <random>

//Mountain::Mountain() : random_engine(hardware_random_generator()), distribution_used(0.0,1.0) {
Mountain::Mountain(){
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
    //printTempDebugInfo();
}

void Mountain::printTempDebugInfo() {
    IndexInterval testInterval{getRelevantMountainSection(10.2, 13.6)};
    std::cout << "Mountain test indices: " << testInterval.start_index << ", "
              << testInterval.end_index << std::endl;
    std::cout << "left point coords: " << getVertex(testInterval.start_index).x
              << ", " << getVertex(testInterval.start_index).y << std::endl;
    std::cout << "right point coords: " << getVertex(testInterval.end_index).x
              << ", " << getVertex(testInterval.end_index).y << std::endl;
}

Position Mountain::getVertex(size_t index) {
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
    /*float leftmost_x = getVertex(0).x;
    IndexInterval returnvalue;
    returnvalue.start_index =
        (std::size_t)std::floor((min_x - leftmost_x) / SECTION_WIDTH);
    returnvalue.end_index =
        (std::size_t)(std::ceil((max_x - leftmost_x) / SECTION_WIDTH) + 1);
    return returnvalue;*/
}

void Mountain::generateNewChunk() {
    std::cout<<"Chunk generated" << std::endl;
    generateSlope();
    generateTerrainRecursive(start_of_circular_array, start_of_circular_array+NUM_SECTIONS_PER_CHUNK-1, 70);
    start_of_circular_array =
        (start_of_circular_array + NUM_SECTIONS_PER_CHUNK) % NUMBER_OF_VERTICES;
}

void Mountain::generateSlope() {
    int num_points_to_generate = NUM_SECTIONS_PER_CHUNK;
    const std::size_t array_size = landscape_fixpoints_circular_array.size();
    const std::size_t index_rightest_vertice =
        (start_of_circular_array + array_size - 1) % array_size;

    float current_x{getVertex(index_rightest_vertice).x};
    float current_y{getVertex(index_rightest_vertice).y};
    for (int i = 0; i < num_points_to_generate; i++) {
        current_x += SECTION_WIDTH;
        current_y += SECTION_WIDTH * SLOPE;
        landscape_fixpoints_circular_array[(start_of_circular_array + i) %
                                           array_size] =
            Position{current_x, current_y};
    }
}

IndexInterval Mountain::getIndexIntervalOfEntireMountain() {
    IndexInterval returnvalue;
    returnvalue.start_index = start_of_circular_array;
    returnvalue.end_index = (start_of_circular_array - 1 + NUMBER_OF_VERTICES);
    return returnvalue;
}

IndexInterval Mountain::getLatestChunk() {
    IndexInterval returnvalue;
    returnvalue.end_index = start_of_circular_array;
    returnvalue.start_index = (start_of_circular_array -
                               NUM_SECTIONS_PER_CHUNK + NUMBER_OF_VERTICES) %
                              NUMBER_OF_VERTICES;
    if (returnvalue.start_index > returnvalue.start_index) {
        returnvalue.end_index += NUMBER_OF_VERTICES;
    }
    return returnvalue;
}

void Mountain::generateTerrainRecursive(std::size_t leftIndex, std::size_t rightIndex, float displacement){
    if(leftIndex+1 == rightIndex) return;
    //if(leftIndex == rightIndex) return;

    std::random_device hardware_random_generator;
    std::mt19937  random_engine(hardware_random_generator());
    std::uniform_real_distribution<double> distribution_used(0.0,1.0);

    std::size_t midIndex = (leftIndex+ rightIndex)/2;   //rounding down is fine
    float change = (distribution_used(random_engine)*2 -1) * displacement;
    landscape_fixpoints_circular_array[(midIndex+NUMBER_OF_VERTICES)%NUMBER_OF_VERTICES].y =
    (landscape_fixpoints_circular_array[(leftIndex+NUMBER_OF_VERTICES)%NUMBER_OF_VERTICES].y +
         landscape_fixpoints_circular_array[(rightIndex+NUMBER_OF_VERTICES)%NUMBER_OF_VERTICES].y)/2 +
        change;
    displacement = Mountain::ROUGHNESS_TERRAIN * displacement;
    generateTerrainRecursive(leftIndex, midIndex, displacement);
    generateTerrainRecursive(midIndex, rightIndex, displacement);
}