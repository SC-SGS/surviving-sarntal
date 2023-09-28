#include "rockSpawnStuff.h"

namespace rockSpawnStuff {

int computeNumRocksToSpawn(RockSpawnPhase rockSpawnPhase,
                           SpawnData *spawnData) {
    // determine num_rocks_to_spawn
    int num_rocks_to_spawn{1};
    if (rockSpawnPhase == rockBatches || rockSpawnPhase == explosiveBatches) {
        spawnData->batch_modulo_count++;
        if (spawnData->batch_modulo_count >= 3) {
            spawnData->batch_modulo_count = 0;
            num_rocks_to_spawn = 3;
        }
    }
    return num_rocks_to_spawn;
}

float rockSpawnTimeFromPhase(RockSpawnPhase rockSpawnPhase) {
    if (rockSpawnPhase == veryBeginning) {
        return 1000.;
    } else if (rockSpawnPhase == irregularRocks) {
        return 5.;
    } else if (rockSpawnPhase == regularRocks) {
        return 3.;
    } else if (rockSpawnPhase == rockBatches) {
        return 4.;
    } else {
        return 4.;
    }
}

RockSpawnPhase determineRockSpawnPhase(double gameTime) {
    if (gameTime < 5.) {
        return veryBeginning;
    } else if (gameTime < 20.) {
        return irregularRocks;
    } else if (gameTime < 40.) {
        return regularRocks;
    } else if (gameTime < 80.) {
        return rockBatches;
    } else {
        return explosiveBatches;
    }
}

} // namespace rockSpawnStuff