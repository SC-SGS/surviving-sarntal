//
// Created by Aleksis Vezenkov on 02.05.24.
//

#include "../Mountain.h"
#include "iostream"
#include <mutex>
#include <random>

MountainClass::MountainClass() {
    std::cout << "Mountain gets constructed" << std::endl;

    // create points and chunks corresponding to a simple ramp
    floatType currentX = -MOUNTAIN_WIDTH;
    floatType currentY = 0.;

    // only initialize a horizontal line
    for (auto &pos : landscapeFixpointCircularArray) {
        pos.x = currentX;
        pos.y = currentY;
        currentX += SECTION_WIDTH;
    }
}

MountainClass::~MountainClass() { std::cout << "Monster destroyed" << std::endl; }

// Mountain::Mountain() : random_engine(hardware_random_generator()),
// distribution_used(0.0,1.0) {

void MountainClass::printTempDebugInfo() const {
    IndexIntervalNew testInterval{getRelevantMountainSection(10.2, 13.6)};
    std::cout << "Mountain test indices: " << testInterval.startIndex << ", " << testInterval.endIndex << std::endl;
    std::cout << "left point coords: " << getVertex(testInterval.startIndex).x << ", "
              << getVertex(testInterval.startIndex).y << std::endl;
    std::cout << "right point coords: " << getVertex(testInterval.endIndex).x << ", "
              << getVertex(testInterval.endIndex).y << std::endl;
}

Position MountainClass::getVertex(const size_t index) const {
    return landscapeFixpointCircularArray[index % NUMBER_OF_VERTICES];
}
Position MountainClass::getVertex(const int index) const {
    return landscapeFixpointCircularArray[(index + NUMBER_OF_VERTICES) % NUMBER_OF_VERTICES];
}

IndexIntervalNew MountainClass::getRelevantMountainSection(const floatType minX, const floatType maxX) {
    IndexIntervalNew relevantMountainSection{};
    relevantMountainSection.startIndex =
        static_cast<std::size_t>(std::floor(minX / SECTION_WIDTH)) % NUMBER_OF_VERTICES;
    relevantMountainSection.endIndex =
        static_cast<std::size_t>(std::ceil(maxX / SECTION_WIDTH) + 1) % NUMBER_OF_VERTICES;
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
    constexpr int NUM_POINTS_TO_GENERATE = NUM_SECTIONS_PER_CHUNK;
    const std::size_t arraySize = this->landscapeFixpointCircularArray.size();
    const std::size_t indexRightestVertex = (this->startOfCircularArray + arraySize - 1) % arraySize;

    floatType currentX{getVertex(indexRightestVertex).x};
    floatType currentY{getVertex(indexRightestVertex).y};
    for (int i = 0; i < NUM_POINTS_TO_GENERATE; i++) {
        currentX += SECTION_WIDTH;
        currentY += SECTION_WIDTH * SLOPE;
        landscapeFixpointCircularArray[(startOfCircularArray + i) % arraySize] = Position{currentX, currentY};
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

void MountainClass::generateTerrainRecursive(const std::size_t leftIndex, const std::size_t rightIndex,
                                             floatType displacement) {
    if ((leftIndex + 1 == rightIndex) || (leftIndex == rightIndex)) {
        return;
    }

    if (leftIndex + 32 >= rightIndex) {
        displacement = 0;
    }

    const std::size_t midIndex = (leftIndex + rightIndex) / 2; // rounding down is fine
    const floatType change = computeDisplacementChange(displacement);

    updateMidpoint(leftIndex, rightIndex, midIndex, change);
    displacement = MountainClass::ROUGHNESS_TERRAIN * displacement;

    generateTerrainRecursive(leftIndex, midIndex, displacement);
    generateTerrainRecursive(midIndex, rightIndex, displacement);
}

floatType MountainClass::computeDisplacementChange(const floatType displacement) {
    std::random_device hardwareRandomGenerator;
    std::mt19937 randomEngine(hardwareRandomGenerator());
    std::uniform_real_distribution<floatType> distributionUsed(0.0, 1.0);

    return (distributionUsed(randomEngine) * 2 - 1) * displacement;
}

void MountainClass::updateMidpoint(const std::size_t leftIndex, const std::size_t rightIndex,
                                   const std::size_t midIndex, const floatType change) {
    landscapeFixpointCircularArray[(midIndex + NUMBER_OF_VERTICES) % NUMBER_OF_VERTICES].y =
        (landscapeFixpointCircularArray[(leftIndex + NUMBER_OF_VERTICES) % NUMBER_OF_VERTICES].y +
         landscapeFixpointCircularArray[(rightIndex + NUMBER_OF_VERTICES) % NUMBER_OF_VERTICES].y) /
            2 +
        change;
}

float MountainClass::getYPosFromX(const float xPos) const {
    const IndexIntervalNew interval = MountainClass::getRelevantMountainSection(xPos, xPos);
    std::size_t closestIndices[] = {interval.startIndex, interval.endIndex};
    auto closestLeftDistance = std::abs(this->getVertex(interval.startIndex).x - xPos);
    auto closestRightDistance = std::abs(this->getVertex(interval.endIndex).x - xPos);

    for (auto j = interval.startIndex; j < interval.endIndex; j++) {
        const auto mountainVertex = this->getVertex(j);
        const auto currentDistance = mountainVertex.x - xPos;

        if (currentDistance < 0 && std::abs(currentDistance) < closestLeftDistance) {
            closestIndices[0] = j;
            closestLeftDistance = std::abs(currentDistance);
        } else if (currentDistance > 0 && std::abs(currentDistance) < closestRightDistance) {
            closestIndices[1] = j;
            closestRightDistance = std::abs(currentDistance);
        }
    }

    const auto vertexLeft = this->getVertex(closestIndices[0]);
    const auto vertexRight = this->getVertex(closestIndices[1]);

    return linearInterpolation(xPos, vertexLeft, vertexRight);
}

float MountainClass::linearInterpolation(const float xPos, const Position left, const Position right) {
    return ((xPos - left.x) * right.y + (right.x - xPos) * left.y) / (right.x - left.x);
}

/*void MountainClass::interpolate(std::size_t leftIndex, std::size_t rightIndex) {
    const auto leftVert = getVertex(leftIndex);
    const auto rightVert = getVertex(rightIndex);
    const floatType slope = (leftVert.y - rightVert.y) / (rightVert.x - leftVert.x);
    for (int i = 1; i < rightIndex - leftIndex; i++) {
        landscape_fixpoints_circular_array[(leftIndex + i) % NUMBER_OF_VERTICES].y =
            static_cast<floatType>(i) * slope + getVertex(leftIndex).y;
    }
}*/
