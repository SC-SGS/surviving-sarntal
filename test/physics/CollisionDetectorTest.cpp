//
// Created by bjoern on 9/22/24.
//

#include "CollisionDetectorTest.h"
#include "../../src/geometry/AABB.h"
#include "../../src/physics/physicists/CollisionDetector.hpp"

CollisionDetectorTest::CollisionDetectorTest()
    : rightTriangle(std::vector<Vector>{{0, 0}, {3, 0}, {3, 4}}),
      rightTriangleBodySpace(std::vector<Vector>{{-2, -4 / 3.0}, {1, -4 / 3.0}, {1, 8 / 3.0}}),
      square(std::vector<Vector>{{-0.5, -0.5}, {0.5, -0.5}, {0.5, 0.5}, {-0.5, 0.5}}),
      rectangle(std::vector<Vector>{{-3, -4}, {3, -4}, {3, 4}, {-3, 4}}) {}

TEST_F(CollisionDetectorTest, PolygonsCollideWithAABBs1) {
    AABB aabb1{{0, 0}, {1, 1}};
    AABB aabb2{{0, 0}, {1, 1}};
    ASSERT_TRUE(CollisionDetector::polygonsCollide(aabb1, aabb2).isCollision);
}

TEST_F(CollisionDetectorTest, PolygonsCollideWithAABBs2) {
    AABB aabb1{{0, 0}, {1, 1}};
    AABB aabb2{{1, 1}, {2, 2}};
    ASSERT_FALSE(CollisionDetector::polygonsCollide(aabb1, aabb2).isCollision);
}

TEST_F(CollisionDetectorTest, PolygonsCollideWithAABBs3) {
    AABB aabb1{{0, 0}, {1, 1}};
    AABB aabb2{{0.3, 0.75}, {2, 2}};
    const PolygonCollisionObject coll = CollisionDetector::polygonsCollide(aabb1, aabb2);
    ASSERT_TRUE(coll.isCollision);
    ASSERT_EQ(0.25, coll.collisionDepth);
    ASSERT_TRUE(coll.collisionDirection.approxEq(Vector{0, -1}, NUMERIC_EPSILON));
}

TEST_F(CollisionDetectorTest, PolygonsCollideWithAABBs4) {
    AABB aabb1{{0, 0}, {1, 1}};
    AABB aabb2{{0.3, 0.75}, {1, 1}};
    const PolygonCollisionObject coll = CollisionDetector::polygonsCollide(aabb1, aabb2);
    ASSERT_TRUE(coll.isCollision);
    ASSERT_EQ(0.25, coll.collisionDepth);
    ASSERT_TRUE(coll.collisionDirection.approxEq(Vector{0, -1}, NUMERIC_EPSILON));
}

TEST_F(CollisionDetectorTest, PolygonsCollideWithAABBs5) {
    AABB aabb1{{0, 0}, {1, 1}};
    AABB aabb2{{1.5, 1.5}, {2, 2}};
    const PolygonCollisionObject coll = CollisionDetector::polygonsCollide(aabb1, aabb2);
    ASSERT_FALSE(coll.isCollision);
}

TEST_F(CollisionDetectorTest, PolygonsCollideWithAABBs6) {
    AABB aabb1{{26.222147, 12.9668865}, {28.1420898, 14.4757252}};
    AABB aabb2{{2.82922125, -168.061172}, {22.0669155, 10.7332811}};
    const PolygonCollisionObject coll = CollisionDetector::polygonsCollide(aabb1, aabb2);
    ASSERT_FALSE(coll.isCollision);
}
