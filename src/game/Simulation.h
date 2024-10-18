//
// Created by bjoern on 10/9/24.
//

#ifndef SIMULATION_H
#define SIMULATION_H

#include "../geometry/DynamicConvexPolygon.h"
#include "../utilities/vector.h"

#include <memory>

class World;
class Rock;

struct SimulationProperties {
    // how long the simulation is run and profiling values are collected
    double duration{};
    // how quickly rocks are spawned after one another
    double speed{};
    size_t maxNumberOfRocks{};
    Vector dropPosition{};
};

/**
 * A rigid body simulation that continuously drops more and more rocks down from [0, 100]
 */
class Simulation {

  public:
    Simulation(World &world, const SimulationProperties &simProperties);

    /**
     * Initializes this simulation.
     */
    void init();

    /**
     * Progresses the simulation by one step.
     *
     * @return whether the simulation is still running
     */
    bool tick();

  private:
    const SimulationProperties simProperties;
    double startTime{};
    double timeOfLastSpawn{};
    size_t numberOfSpawnedRocks = 0;
    World &world;
    std::shared_ptr<Rock> rockBlueprint{};
    DynamicProperties rockSpawnState{};

    Vector getLinearMomentumFromSpeed() const;

    void spawnRock();

    bool shouldSpawnAgain(double time) const;
};

#endif // SIMULATION_H
