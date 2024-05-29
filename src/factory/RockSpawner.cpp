//
// Created by Anietta Weckauff on 13.05.24.
//

#include "RockSpawner.h"

SpawnData RockSpawner::spawnData = {0, 0, 0};

void RockSpawner::spawnRocks(World *world, SpawnData *spawnData) {
    // TODO figure out camera stuff
    // auto camera =
    // it.world().lookup("Camera").get_mut<graphics::Camera2DComponent>();

    auto gameTime = GetTime();
    RockSpawnPhase rockSpawnPhase = determineRockSpawnPhase(gameTime);
    float timeBetweenRockspawns = rockSpawnTimeFromPhase(rockSpawnPhase);

    if (gameTime > spawnData->rockSpawnTime + timeBetweenRockspawns) {
        spawnData->rockSpawnTime = spawnData->rockSpawnTime + timeBetweenRockspawns;

        // compute spawn Basepoint
        // offset on x-axis by -300 to spawn visible on screen
        constexpr float DEBUG_MAKE_SPAWN_VISIBLE_OFFSET = 400.;

        // TODO figure out camera stuff
        // const float spawn_x_coord = camera->target.x +
        // ((float)graphics::SCREEN_WIDTH) / 2 + 100;
        // //-DEBUG_MAKE_SPAWN_VISIBLE_OFFSET;

        const float spawnXCoord = 0;
        Position spawnBasepoint = world->getMountain().getVertex(
            MountainClass::getRelevantMountainSection(spawnXCoord, spawnXCoord).startIndex);
        // spawn rocks offset by constant amount above mountain
        spawnBasepoint.y += 350.;

        int numRocksToSpawn = computeNumRocksToSpawn(rockSpawnPhase, spawnData);
        const std::vector<Position> offsetsAdditionalRocks{
            {0., 0.}, {MAX_ROCK_SIZE + 5., MAX_ROCK_SIZE * 2 + 10.}, {-MAX_ROCK_SIZE - 5., MAX_ROCK_SIZE * 2 + 10.}};

        for (int i{0}; i < numRocksToSpawn; i++) {
            // NOLINTBEGIN
            double r = ((double)std::rand() / (RAND_MAX));
            // NOLINTEND
            float radius = ((float)r) * (MAX_ROCK_SIZE - MIN_ROCK_SIZE) + MIN_ROCK_SIZE;

            constexpr float CONST_VEL_COMPONENT{-300.};
            // NOLINTBEGIN
            float randomVelocityComponent{((((float)std::rand()) / RAND_MAX) - 0.5f) * 200.f};
            // NOLINTEND

            std::cout << "rock spawned" << std::endl;

            Vector velocity = {CONST_VEL_COMPONENT + randomVelocityComponent, 0};
            Rotation rotation = {0.0f, 0.0f};

            Vector position = {spawnBasepoint.x + offsetsAdditionalRocks[i].x,
                               spawnBasepoint.y + offsetsAdditionalRocks[i].y};

            RockClass newRock(velocity, rotation, radius, position);

            world->addRock(&newRock);

            if (rockSpawnPhase == EXPLOSIVE_BATCHES) {
                spawnData->explosiveRockModuloCount++;
                if (spawnData->explosiveRockModuloCount >= 10) {
                    spawnData->explosiveRockModuloCount = 0;
                    // TODO what about this?
                    // rock_entity.add<Exploding>();
                }
            }
        }
    }
}
int RockSpawner::computeNumRocksToSpawn(RockSpawnPhase rockSpawnPhase, SpawnData *spawnData) {
    int numRocksToSpawn{1};
    if (rockSpawnPhase == ROCK_BATCHES || rockSpawnPhase == EXPLOSIVE_BATCHES) {
        spawnData->batchModuloCount++;
        if (spawnData->batchModuloCount >= 3) {
            spawnData->batchModuloCount = 0;
            numRocksToSpawn = 3;
        }
    }
    return numRocksToSpawn;
}
float RockSpawner::rockSpawnTimeFromPhase(RockSpawnPhase rockSpawnPhase) {
    if (rockSpawnPhase == VERY_BEGINNING) {
        return 1000.;
    } else if (rockSpawnPhase == IRREGULAR_ROCKS) {
        return 5.;
    } else if (rockSpawnPhase == REGULAR_ROCKS) {
        return 3.;
    } else { // rockSpawnPhase == ROCK_BATCHES
        return 4.;
    }
}
RockSpawnPhase RockSpawner::determineRockSpawnPhase(double gameTime) {
    if (gameTime < 5.) {
        return VERY_BEGINNING;
    } else if (gameTime < 20.) {
        return IRREGULAR_ROCKS;
    } else if (gameTime < 40.) {
        return REGULAR_ROCKS;
    } else if (gameTime < 80.) {
        return ROCK_BATCHES;
    } else {
        return EXPLOSIVE_BATCHES;
    }
}
