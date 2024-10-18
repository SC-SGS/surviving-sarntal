//
// Created by bjoern on 10/11/24.
//

#ifndef RECORDER_H
#define RECORDER_H
#include "../physics/PhysicsEngine.hpp"

class PhysicsEngine;

/**
 * Helper class used in devMode to record physics time step sizes.
 * Necessary to produce plots for the report.
 */
class Recorder {
  public:
    explicit Recorder(PhysicsEngine &physicsEngine);

    void start();

    void stop();

    void printResults() const;

    void toggle();

    bool isRecording() const;

    void tick();

    /**
     * Saves to
     * ../../report/plots/sim.csv and
     * ../../report/plots/simFPS.csv
     */
    void saveResultsToCSV() const;

  private:
    PhysicsEngine &physicsEngine;
    std::list<Record> records;
    std::list<std::tuple<double, double>> fpsRecords;
    bool recording = false;

    static void printRecord(const Record &record);

    void savePhysicsResultsToCSV() const;

    void saveFPSResultsToCSV() const;
};

#endif // RECORDER_H
