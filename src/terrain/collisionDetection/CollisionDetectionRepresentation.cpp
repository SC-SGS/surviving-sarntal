//
// Created by felix-roehr on 9/10/24.
//

#include "CollisionDetectionRepresentation.hpp"

CollisionDetectionRepresentation::CollisionDetectionRepresentation(
    const std::vector<std::shared_ptr<StaticPolyline>> &polySections, AxisAlignedBoundingBox &boundingBox)
    : polylineSectionsSortedT(polySections), boundingBox(boundingBox) {
    this->sortLeftBorder();
    this->sortRightBorder();
}

const AxisAlignedBoundingBox &CollisionDetectionRepresentation::getBoundingBox() const { return this->boundingBox; }

void CollisionDetectionRepresentation::sortLeftBorder() {
    this->polylineSectionsSortedLeftBorder = this->polylineSectionsSortedT;
    std::sort(
        this->polylineSectionsSortedLeftBorder.begin(), this->polylineSectionsSortedLeftBorder.end(),
        [&](const std::shared_ptr<StaticPolyline> &firstSection, const std::shared_ptr<StaticPolyline> &secondSection) {
            return firstSection->getBoundingBox().isSmallerLeftBorder(secondSection->getBoundingBox());
        });
}

void CollisionDetectionRepresentation::sortRightBorder() {
    this->polylineSectionsSortedRightBorder = this->polylineSectionsSortedT;
    std::sort(
        this->polylineSectionsSortedRightBorder.begin(), this->polylineSectionsSortedRightBorder.end(),
        [&](const std::shared_ptr<StaticPolyline> &firstSection, const std::shared_ptr<StaticPolyline> &secondSection) {
            return firstSection->getBoundingBox().isSmallerRightBorder(secondSection->getBoundingBox());
        });
}

std::vector<std::shared_ptr<StaticPolyline>>
CollisionDetectionRepresentation::calculateRelevantSections(const AxisAlignedBoundingBox &boundingBox) {
    std::vector<std::shared_ptr<StaticPolyline>> sectionsLeftBorder = this->calculateRelevantSectionsX(
        this->polylineSectionsSortedLeftBorder, &AxisAlignedBoundingBox::isSmallerLeftBorder, boundingBox);
    std::vector<std::shared_ptr<StaticPolyline>> sectionsRightBorder = this->calculateRelevantSectionsX(
        this->polylineSectionsSortedRightBorder, &AxisAlignedBoundingBox::isSmallerRightBorder, boundingBox);
    std::vector<std::shared_ptr<StaticPolyline>> sectionsX = sectionsLeftBorder;

    for (const auto &section : sectionsRightBorder) {
        if (!std::any_of(sectionsX.cbegin(), sectionsX.cend(),
                         [&](const std::shared_ptr<StaticPolyline> &oldSection) { return section == oldSection; })) {
            sectionsX.push_back(section);
        }
    }

    std::vector<std::shared_ptr<StaticPolyline>> sections = {};
    for (const auto &section : sectionsX) {
        if (section->getBoundingBox().overlaps(boundingBox)) {
            sections.push_back(section);
        }
    }

    return sections;
}

std::vector<std::shared_ptr<StaticPolyline>> CollisionDetectionRepresentation::calculateRelevantSectionsX(
    const std::vector<std::shared_ptr<StaticPolyline>> &sortedSections,
    const IsSmallerComparator isSmallerComparator,
    const AxisAlignedBoundingBox &boundingBox) {
    int index = (static_cast<int>(sortedSections.size() - 1)) / 2;
    int min = 0;
    int max = static_cast<int>(sortedSections.size()) - 1;
    std::vector<std::shared_ptr<StaticPolyline>> sectionsX = {};
    while (!sortedSections.at(index)->getBoundingBox().overlapsX(boundingBox) && min <= max) {
        const AxisAlignedBoundingBox sectionBoundingBox = sortedSections.at(index)->getBoundingBox();
        if ((boundingBox.*isSmallerComparator)(sectionBoundingBox)) {
            max = index - 1;
        } else {
            min = index + 1;
        }
        index = min + (max - min) / 2;
    }
    if (min <= max) {
        sectionsX.push_back(sortedSections.at(index));
        int additionalIndex = index - 1;
        while (additionalIndex >= 0 && sortedSections.at(additionalIndex)->getBoundingBox().overlapsX(boundingBox)) {
            sectionsX.push_back(sortedSections.at(additionalIndex));
            additionalIndex--;
        }
        additionalIndex = index + 1;
        while (additionalIndex < sortedSections.size() &&
               sortedSections.at(additionalIndex)->getBoundingBox().overlapsX(boundingBox)) {
            sectionsX.push_back(sortedSections.at(additionalIndex));
            additionalIndex++;
        }
    }
    return sectionsX;
}

std::vector<std::shared_ptr<Intersection>> CollisionDetectionRepresentation::calculateIntersections(Line &line) {
    AxisAlignedBoundingBox lineBoundingBox = AxisAlignedBoundingBox::getBoundingBox(line);
    std::vector<std::shared_ptr<StaticPolyline>> relevantSections = this->calculateRelevantSections(lineBoundingBox);
    std::vector<std::shared_ptr<Intersection>> intersections = {};
    for (const auto &polyline : relevantSections) {
        std::vector<std::shared_ptr<Intersection>> newIntersections = polyline->calculateIntersections(line);
        intersections.insert(intersections.cend(), newIntersections.cbegin(), newIntersections.cend());
    }
    return intersections;
}

std::vector<std::shared_ptr<StaticPolyline>>
CollisionDetectionRepresentation::calculateRelevantSectionsContinuous(const AxisAlignedBoundingBox &boundingBox) {
    std::vector<std::shared_ptr<StaticPolyline>> sections = this->calculateRelevantSections(boundingBox);
    std::vector<std::shared_ptr<StaticPolyline>> sectionsContinuousOrdered = {};

    int maxIndex = 0;
    int minIndex = static_cast<int>(this->polylineSectionsSortedT.size()) - 1;

    auto maxIter = std::max_element(
        sections.begin(), sections.end(),
        [](const std::shared_ptr<StaticPolyline> &first, const std::shared_ptr<StaticPolyline> &second) {
            return first->getIndex() < second->getIndex();
        });
    auto minIter = std::min_element(
        sections.begin(), sections.end(),
        [](const std::shared_ptr<StaticPolyline> &first, const std::shared_ptr<StaticPolyline> &second) {
            return first->getIndex() < second->getIndex();
        });

    if (minIter != sections.end() && maxIter != sections.end()) {
        minIndex = (*minIter)->getIndex().value();
        maxIndex = (*maxIter)->getIndex().value();
    }
    for (int index = std::max(0, minIndex); index <= maxIndex; index++) {
        sectionsContinuousOrdered.push_back(this->polylineSectionsSortedT.at(index));
    }
    return sectionsContinuousOrdered;
}
