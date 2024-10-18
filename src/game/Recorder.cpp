//
// Created by bjoern on 10/11/24.
//

#include "Recorder.h"
#include <fstream>

Recorder::Recorder(PhysicsEngine &physicsEngine) : physicsEngine(physicsEngine) {}

void Recorder::start() {
    if (this->recording) {
        this->stop();
    }
    this->physicsEngine.startRecording();
    this->recording = true;
}

void Recorder::stop() {
    if (!this->recording) {
        return;
    }
    this->records.emplace_back(this->physicsEngine.stopRecording());
    this->recording = false;
}

void Recorder::toggle() {
    if (this->recording) {
        this->stop();
    } else {
        this->start();
    }
}

bool Recorder::isRecording() const { return this->recording; }

void Recorder::tick() { this->fpsRecords.emplace_back(GetTime(), GetFPS()); }

void Recorder::printResults() const {
    spdlog::info("=======================================\n\n");
    for (const auto &record : this->records) {
        printRecord(record);
        spdlog::info("=======================================\n\n");
    }
}

void Recorder::printRecord(const Record &record) {
    spdlog::info("Avg. Time: {}s", record.avgStepTime);
    std::string plotTimesX = "[ ";
    std::string plotTimesY = "[ ";
    size_t index = 0;
    for (const auto times : record.stepTimes) {
        plotTimesX.append(std::to_string(std::get<0>(times)));
        plotTimesY.append(std::to_string(std::get<1>(times)));
        if (index < record.stepTimes.size() - 1) {
            plotTimesX.append(", ");
            plotTimesY.append(", ");
        } else {
            plotTimesX.append("]");
            plotTimesY.append("]");
        }
        index++;
    }
    spdlog::info("Runtimes: {}", plotTimesX);
    spdlog::info("Step sizes: {}", plotTimesY);
}

void Recorder::saveResultsToCSV() const {
    this->savePhysicsResultsToCSV();
    this->saveFPSResultsToCSV();
}

void Recorder::savePhysicsResultsToCSV() const {
    const std::string filename = "../../report/plots/sim.csv";
    // Open file for writing
    std::ofstream file(filename);

    if (!file.is_open()) {
        spdlog::error("Error opening file for writing.");
        return;
    }

    // Write the header of the CSV file
    file << "simTime,stepTime\n"; // Adjust header names if needed (e.g. for bar graph)

    // Iterate through each record and write stepTimes (x and y values)
    for (const auto &record : this->records) {
        for (const auto &times : record.stepTimes) {
            // Write x and y values to the CSV file
            file << std::get<0>(times) << "," << std::get<1>(times) << "\n";
        }
    }

    file.close();
    spdlog::info("Data saved to {}", filename);
}

void Recorder::saveFPSResultsToCSV() const {
    const std::string filename = "../../report/plots/simFPS.csv";
    // Open file for writing
    std::ofstream file(filename);

    if (!file.is_open()) {
        spdlog::error("Error opening file for writing.");
        return;
    }

    // Write the header of the CSV file
    file << "simTime,fps\n"; // Adjust header names if needed (e.g. for bar graph)

    // Iterate through each record and write stepTimes (x and y values)
    for (const auto &times : this->fpsRecords) {
        // Write x and y values to the CSV file
        file << std::get<0>(times) << "," << std::get<1>(times) << "\n";
    }

    file.close();
    spdlog::info("Data saved to {}", filename);
}