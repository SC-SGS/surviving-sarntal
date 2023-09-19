//
// Created by johnny on 18.09.23.
//

#include <vector>
#include "mountain.h"
#include "iostream"

Mountain::Mountain() {
    std::cout << "Mountain gets constructed" << std::endl;

    //create points and chunks corresponding to a simple ramp
    double current_x = 0.;
    double current_y = 0.;
    for(int i = 0; i < landscape_fixpoints_circular_array.size(); i++){
        landscape_fixpoints_circular_array[i].x = current_x;
        landscape_fixpoints_circular_array[i].y = current_y;
        current_x += section_width;
        current_y += section_width*slope;
    }
    printTempDebugInfo();
    generateNewChunk();
    generateNewChunk();
    printTempDebugInfo();
}

/**
 * Temporary helper function, do not touch
 */
void Mountain::printTempDebugInfo(){
    IndexInterval testInterval{ getRelevantMountainSection(10.2, 13.6)};
    std::cout << "Mountain test indices: " << testInterval.start_indice << ", " << testInterval.end_indice << std::endl;
    std::cout << "left point coords: " << getFixpoint(testInterval.start_indice).x << ", " << getFixpoint(testInterval.start_indice).y << std::endl;
    std::cout << "right point coords: " << getFixpoint(testInterval.end_indice).x << ", " << getFixpoint(testInterval.end_indice).y << std::endl;
}

/**
 *
 * @param index
 * @return
 */
Position Mountain::getFixpoint(size_t index) {
    return landscape_fixpoints_circular_array[(index+start_of_circular_array)%landscape_fixpoints_circular_array.size()];
}

/**
 *
 * @param min_x
 * @param max_x
 * @return
 */
IndexInterval Mountain::getRelevantMountainSection(float min_x, float max_x){
    float leftmost_x = getFixpoint(0).x;
    IndexInterval returnvalue;
    returnvalue.start_indice = (std::size_t) std::floor((min_x- leftmost_x)/section_width);
    returnvalue.end_indice = (std::size_t) (std::ceil((max_x - leftmost_x)/section_width) + 1);
    return returnvalue;
}

void Mountain::generateNewChunk() {
    int num_points_to_generate =(int) (std::round(chunk_width/section_width));

    float current_x{getFixpoint(number_of_fixpoints-1).x};
    float current_y{getFixpoint(number_of_fixpoints-1).y};
    for(int i = 0; i < num_points_to_generate; i++){
        current_x += section_width;
        current_y += section_width*slope;
        landscape_fixpoints_circular_array[(start_of_circular_array + i) % landscape_fixpoints_circular_array.size()] =
                Position{current_x, current_y};
    }

    start_of_circular_array = (start_of_circular_array + num_points_to_generate)%landscape_fixpoints_circular_array.size();
}
