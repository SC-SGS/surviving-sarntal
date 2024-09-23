//
// Created by Aleksis Vezenkov on 05.08.24.
//

#ifndef SURVIVING_SARNTAL_DEVMODE_HPP
#define SURVIVING_SARNTAL_DEVMODE_HPP

#include "../spawner/PolygonGenerator.h"
#include "Game.hpp"

struct TestCaseDto {
    std::string name;
    std::vector<Rock> rocks;
};

class DevMode {

  public:
    DevMode(World &world,
            Renderer &renderer,
            PhysicsEngine &physicsEngine,
            Spawner &spawner,
            AudioService &audioService,
            InputHandler &inputHandler,
            GameConstants &gameConstants,
            Camera2D &camera);
    ~DevMode() = default;
    void run();
    bool debugMode = false;

  private:
    World &world;
    Renderer &renderer;
    PhysicsEngine &physicsEngine;
    Spawner &spawner;
    AudioService &audioService;
    InputHandler &inputHandler;
    GameConstants &gameConstants;
    YAML::Node configTest;
    std::unordered_map<ItemType, ItemDto> items;
    Camera2D &camera;
    float rockSize = 10.0;
    std::vector<TestCaseDto> testCases;
    std::string currentTestCase = "Dev Mode";
    PolygonGenerator polyGen;

    // Helper functions
    void mainLoop();
    Vector getTransformedMousePosition() const;
    void loadTestConfig();
    std::vector<TestCaseDto> loadTestCases();
    void clearAllItems();
    void clearAllRocks();
    void increaseRockSize(float increment);

    void spawnRockAtMouse();
    void spawnItemAtMouse(int itemId);
    void startTestCase(int testCase);
    void spawnRocks(const std::vector<Rock> &rocks);
    Vector getTransformedPosition(const Vector &position) const;
    void drawOnScreen(const char *message);
    TestCaseDto mapToTestCaseDto(const YAML::Node &testCase);
    void loadRocksFromNode(const YAML::Node &rocksNode, TestCaseDto &dto);
    void renderText(const std::string &message) const;
    bool shouldStartTestCase();
    int getTestCaseFromInput();
    void spawnRock(Vector position, Vector velocity);
    void spawnRock(Vector position, Vector velocity, floatType radius);
    Rock generateRock(Vector position, Vector velocity, floatType radius);
};

#endif // SURVIVING_SARNTAL_DEVMODE_HPP
