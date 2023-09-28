#pragma once

namespace rockSpawnStuff {

struct SpawnData {
    float rock_spawn_time{0.};
    int batch_modulo_count{0};
    int explosive_rock_modulo_count{0};
};

enum RockSpawnPhase {
    veryBeginning,
    irregularRocks,
    regularRocks,
    rockBatches,
    explosiveBatches
};

int computeNumRocksToSpawn(RockSpawnPhase rockSpawnPhase, SpawnData *spawnData);

float rockSpawnTimeFromPhase(RockSpawnPhase rockSpawnPhase);

RockSpawnPhase determineRockSpawnPhase(double gameTime);

} // namespace rockSpawnStuff
