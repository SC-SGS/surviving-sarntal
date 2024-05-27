//
// Created by Aleksis Vezenkov on 02.05.24.
//

#include "../Mountain.h"
#include "iostream"
#include <random>

// Mountain::Mountain() : random_engine(hardware_random_generator()),
// distribution_used(0.0,1.0) {
MountainClass::MountainClass() {
    std::cout << "Mountain gets constructed" << std::endl;

    // create points and chunks corresponding to a simple ramp
    float_type currentX = -MOUNTAIN_WIDTH;
    float_type currentY = 0.;

    // only initialize a horizontal line
    for (auto &i : landscapeFixpointCircularArray) {
        i.x = currentX;
        i.y = currentY;
        currentX += SECTION_WIDTH;
    }
}

void MountainClass::printTempDebugInfo() {
    IndexIntervalNew testInterval{getRelevantMountainSection(10.2, 13.6)};
    std::cout << "Mountain test indices: " << testInterval.startIndex << ", " << testInterval.endIndex << std::endl;
    std::cout << "left point coords: " << getVertex(testInterval.startIndex).x << ", "
              << getVertex(testInterval.startIndex).y << std::endl;
    std::cout << "right point coords: " << getVertex(testInterval.endIndex).x << ", "
              << getVertex(testInterval.endIndex).y << std::endl;
}

Position MountainClass::getVertex(size_t index) { return landscapeFixpointCircularArray[index % NUMBER_OF_VERTICES]; }
Position MountainClass::getVertex(int index) {
    return landscapeFixpointCircularArray[(index + NUMBER_OF_VERTICES) % NUMBER_OF_VERTICES];
}

IndexIntervalNew MountainClass::getRelevantMountainSection(float_type minX, float_type maxX) {
    IndexIntervalNew relevantMountainSection{};
    relevantMountainSection.startIndex = ((std::size_t)std::floor(minX / SECTION_WIDTH)) % NUMBER_OF_VERTICES;
    relevantMountainSection.endIndex = ((std::size_t)(std::ceil(maxX / SECTION_WIDTH) + 1)) % NUMBER_OF_VERTICES;
    if (relevantMountainSection.endIndex < relevantMountainSection.startIndex)
        relevantMountainSection.endIndex += NUMBER_OF_VERTICES;

    return relevantMountainSection;
    /*float leftmost_x = getVertex(0).x;
    IndexIntervalNew relevantMountainSection;
    relevantMountainSection.startIndex =
        (std::size_t)std::floor((minX - leftmost_x) / SECTION_WIDTH);
    relevantMountainSection.endIndex =
        (std::size_t)(std::ceil((maxX - leftmost_x) / SECTION_WIDTH) + 1);
    return relevantMountainSection;*/
}

void MountainClass::generateNewChunk() {
    std::cout << "Chunk generated" << std::endl;
    generateSlope();
    generateTerrainRecursive(startOfCircularArray, startOfCircularArray + NUM_SECTIONS_PER_CHUNK - 1, 100);
    startOfCircularArray = (startOfCircularArray + NUM_SECTIONS_PER_CHUNK) % NUMBER_OF_VERTICES;
}

void MountainClass::generateSlope() {
    int numPointsToGenerate = NUM_SECTIONS_PER_CHUNK;
    const std::size_t ARRAY_SIZE = landscapeFixpointCircularArray.size();
    const std::size_t INDEX_RIGHTEST_VERTICE = (startOfCircularArray + ARRAY_SIZE - 1) % ARRAY_SIZE;

    float_type currentX{getVertex(INDEX_RIGHTEST_VERTICE).x};
    float_type currentY{getVertex(INDEX_RIGHTEST_VERTICE).y};
    for (int i = 0; i < numPointsToGenerate; i++) {
        currentX += SECTION_WIDTH;
        currentY += SECTION_WIDTH * SLOPE;
        landscapeFixpointCircularArray[(startOfCircularArray + i) % ARRAY_SIZE] = Position{currentX, currentY};
    }
}

IndexIntervalNew MountainClass::getIndexIntervalOfEntireMountain() const {
    IndexIntervalNew indexInterval{};
    indexInterval.startIndex = startOfCircularArray;
    indexInterval.endIndex = (startOfCircularArray - 1 + NUMBER_OF_VERTICES);
    return indexInterval;
}

IndexIntervalNew MountainClass::getLatestChunk() const {
    IndexIntervalNew latestChunk{};
    latestChunk.endIndex = startOfCircularArray;
    latestChunk.startIndex = (startOfCircularArray - NUM_SECTIONS_PER_CHUNK + NUMBER_OF_VERTICES) % NUMBER_OF_VERTICES;
    if (latestChunk.startIndex > latestChunk.endIndex) {
        latestChunk.endIndex += NUMBER_OF_VERTICES;
    }
    return latestChunk;
}

void MountainClass::generateTerrainRecursive(std::size_t leftIndex, std::size_t rightIndex, float_type displacement) {
    if ((leftIndex + 1 == rightIndex) || (leftIndex == rightIndex)) {
        return;
    }

    if (leftIndex + 32 >= rightIndex) {
        displacement = 0;
    }

    std::size_t midIndex = (leftIndex + rightIndex) / 2; // rounding down is fine
    float_type change = computeDisplacementChange(displacement);

    updateMidpoint(leftIndex, rightIndex, midIndex, change);
    displacement = MountainClass::ROUGHNESS_TERRAIN * displacement;

    generateTerrainRecursive(leftIndex, midIndex, displacement);
    generateTerrainRecursive(midIndex, rightIndex, displacement);
}

float_type MountainClass::computeDisplacementChange(float_type displacement) {
    std::random_device hardwareRandomGenerator;
    std::mt19937 randomEngine(hardwareRandomGenerator());
    std::uniform_real_distribution<float_type> distributionUsed(0.0, 1.0);

    return (distributionUsed(randomEngine) * 2 - 1) * displacement;
}

void MountainClass::updateMidpoint(std::size_t leftIndex, std::size_t rightIndex, std::size_t midIndex,
                                   float_type change) {
    landscapeFixpointCircularArray[(midIndex + NUMBER_OF_VERTICES) % NUMBER_OF_VERTICES].y =
        (landscapeFixpointCircularArray[(leftIndex + NUMBER_OF_VERTICES) % NUMBER_OF_VERTICES].y +
         landscapeFixpointCircularArray[(rightIndex + NUMBER_OF_VERTICES) % NUMBER_OF_VERTICES].y) /
            2 +
        change;
}

void MountainClass::interpolate(std::size_t leftIndex, std::size_t rightIndex) {
    auto leftVert = getVertex(leftIndex);
    auto rightVert = getVertex(rightIndex);
    float_type m = (leftVert.y - rightVert.y) / (rightVert.x - leftVert.x);
    for (int i = 1; i < rightIndex - leftIndex; i++) {
        landscapeFixpointCircularArray[(leftIndex + i) % NUMBER_OF_VERTICES].y =
            static_cast<float_type>(i) * m + getVertex(leftIndex).y;
    }
}
