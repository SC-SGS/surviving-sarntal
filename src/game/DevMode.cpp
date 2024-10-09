//
// Created by Aleksis Vezenkov on 05.08.24.
//

#include "DevMode.hpp"

#include "../spawner/PolygonGenerator.h"

DevMode::DevMode(World &world,
                 Renderer &renderer,
                 PhysicsEngine &physicsEngine,
                 Spawner &spawner,
                 AudioService &audioService,
                 InputHandler &inputHandler,
                 GameConstants &gameConstants,
                 Camera2D &camera)
    : world(world),
      renderer(renderer),
      physicsEngine(physicsEngine),
      spawner(spawner),
      audioService(audioService),
      inputHandler(inputHandler),
      gameConstants(gameConstants),
      camera(camera) {}

void DevMode::init() {
    loadTestConfig();
    std::vector<Vector> groundPoints = ConfigManager::getInstance().getGroundPointsDevMode();
    if (groundPoints.back().x < this->gameConstants.terrainConstants.biomeWidth) {
        groundPoints.push_back({this->gameConstants.terrainConstants.biomeWidth, groundPoints.back().y});
    }
    this->world.getTerrain().initialize(groundPoints);
    this->items = ConfigManager::getInstance().getItems();
    this->world.getHiker().setPosition({0, this->world.getTerrain().mapToClosestTopTerrain(Vector{0.f, 0.f})});
    this->world.getHiker().move({0.f, 0.f});
    this->testCases = loadTestCases();
    spdlog::info("Dev mode was initialized successfully.");
}

void DevMode::run() {
    audioService.playSound("background-music");
    while (!WindowShouldClose()) {
        std::queue<GameEvent> events = this->inputHandler.getEvents();
        this->physicsEngine.update(events);
        this->renderer.draw();
        this->world.updateGameScore();
        this->mainLoop();
        this->renderText(this->currentTestCase);
    }
}

void DevMode::mainLoop() {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        this->spawnRockAtMouse();
    } else if (IsKeyPressed(KEY_N)) {
        this->clearAllItems();
    } else if (IsKeyPressed(KEY_M)) {
        this->clearAllRocks();
    } else if (IsKeyPressed(KEY_PERIOD)) {
        this->increaseRockSize(10.0);
    } else if (IsKeyPressed(KEY_COMMA)) {
        this->increaseRockSize(-10.0);
    } else if (!IsKeyDown(KEY_RIGHT_SHIFT) && IsKeyPressed(KEY_ZERO)) {
        this->spawnItemAtMouse(0);
    } else if (!IsKeyDown(KEY_RIGHT_SHIFT) && IsKeyPressed(KEY_ONE)) {
        this->spawnItemAtMouse(1);
    } else if (!IsKeyDown(KEY_RIGHT_SHIFT) && IsKeyPressed(KEY_TWO)) {
        this->spawnItemAtMouse(2);
    } else if (this->shouldStartTestCase()) {
        const int testCase = this->getTestCaseFromInput();
        this->startTestCase(testCase);
    }
}

void DevMode::spawnRockAtMouse() const {
    Vector position = getTransformedMousePosition();
    Vector velocity = Vector();

    if (IsKeyDown(KEY_RIGHT)) {
        velocity.x = 500.0;
    } else if (IsKeyDown(KEY_LEFT)) {
        velocity.x = -500.0;
    }
    if (IsKeyDown(KEY_UP)) {
        velocity.y = 500.0;
    }
    auto hpos = this->world.getHiker().getPosition();
    spdlog::info("Hiker position: (x: {0}, y: {1}) ", hpos.x, hpos.y);
    spdlog::info("Mouse position: (x: {0}, y: {1}) ", position.x, position.y);

    this->spawnRock(position, velocity);
}

void DevMode::clearAllItems() const { this->world.clearAllItems(); }

void DevMode::clearAllRocks() const { this->world.clearAllRocks(); }

void DevMode::increaseRockSize(const float increment) {
    if (this->rockSize + increment > 0) {
        this->rockSize += increment;
    }
}

void DevMode::spawnItemAtMouse(int itemId) {
    const auto itemType = ItemType(itemId);
    const auto itemDto = this->items[itemType];
    const Item item =
        Item(itemType, getTransformedMousePosition(), gameConstants.itemsConstants.itemBaseHeight, itemDto);
    this->world.addItem(item);
}

bool DevMode::shouldStartTestCase() const {
    const int keyPressed = GetKeyPressed();
    const bool keyPressedIsNum = KEY_ZERO <= keyPressed && keyPressed <= KEY_NINE;
    return IsKeyDown(KEY_RIGHT_SHIFT) && keyPressedIsNum;
}

int DevMode::getTestCaseFromInput() const {
    for (int i = 0; i < 9; i++) {
        if (IsKeyPressed(KEY_ZERO + i)) {
            return i;
        }
    }
    return 0;
}

void DevMode::drawOnScreen(const char *message) const {
    const int fontSize = 40;
    // Calculate the text width and height
    const int textWidth = MeasureText(message, fontSize);
    const int textHeight = fontSize; // Ascent + Descent, but raylib doesn't provide this separately, so using font size

    // Calculate the positions
    const int posX = (GetScreenWidth() - textWidth) / 2;
    const int posY = (GetScreenWidth() - textHeight) / 2;

    DrawText(message, posX, posY, fontSize, RED);
}

Vector DevMode::getTransformedMousePosition() const {
    const Vector2 positionMouse = GetMousePosition();
    Vector position = Vector();
    position.x = positionMouse.x;
    const float pos = GraphicsUtil::transformYCoordinate(camera.target.y) - positionMouse.y +
                      static_cast<float>(GetScreenHeight()) / 2;
    position.y = pos;
    return position;
}

void DevMode::loadTestConfig() {
    std::string configLocation = "../../src/manual-test/configuration-man-test.yaml";

    try {
        this->configTest = YAML::LoadFile(configLocation);
    } catch (...) {
        spdlog::critical(
            "Configuration file for manual test could not be loaded. Check if file with relative location: {} exists.",
            configLocation);
        throw std::runtime_error("Something went wrong when loading the configuration for the manual test.");
    }
    spdlog::info("Configuration for manual test was loaded successfully.");
}

void DevMode::startTestCase(int testCase) {
    if (this->testCases.size() <= testCase) {
        spdlog::critical("Trying to start a test case that has not been loaded.");
        return;
    }
    auto dto = this->testCases[testCase];
    spdlog::info("Starting test case {0} with name {1}", testCase, dto.name);
    this->currentTestCase = dto.name;
    this->clearAllRocks();
    this->clearAllItems();
    this->spawnRocks(dto.rocks);
}

std::vector<TestCaseDto> DevMode::loadTestCases() {
    std::vector<TestCaseDto> testCasesList;
    YAML::Node testCasesNode = configTest["test-cases"];

    if (testCasesNode.IsSequence()) {
        for (auto &&testCaseLocation : testCasesNode) {
            auto location = testCaseLocation.as<std::string>();
            YAML::Node node = YAML::LoadFile(location);
            TestCaseDto dto = mapToTestCaseDto(node);
            testCasesList.push_back(dto);
        }
        spdlog::info("Successfully loaded the test cases from the manual test configuration.");
    } else {
        spdlog::critical("Property test-cases in the manual test configuration is not a valid yaml sequence.");
    }

    return testCasesList;
}

TestCaseDto DevMode::mapToTestCaseDto(const YAML::Node &testCase) const {
    TestCaseDto dto;

    dto.name = testCase["name"].as<std::string>();
    loadRocksFromNode(testCase["rocks"], dto);

    return dto;
}

void DevMode::loadRocksFromNode(const YAML::Node &rocksNode, TestCaseDto &dto) const {
    for (const auto &rockNode : rocksNode) {
        auto position = rockNode["position"].as<std::vector<floatType>>();
        auto velocity = rockNode["velocity"].as<std::vector<floatType>>();
        const auto radius = rockNode["radius"].as<floatType>();

        const auto posVec = Vector{position[0], position[1]};
        const auto velVec = Vector{velocity[0], velocity[1]};

        Rock rock = this->generateRock(posVec, velVec, radius);
        dto.rocks.push_back(rock);
    }
}

void DevMode::spawnRock(const Vector &position, const Vector &velocity) const {
    this->spawnRock(position, velocity, this->rockSize);
}

void DevMode::spawnRock(const Vector &position, const Vector &velocity, const floatType radius) const {
    Rock newRock = this->generateRock(position, velocity, radius);
    this->world.addRock(std::make_shared<Rock>(newRock));
}

Rock DevMode::generateRock(const Vector &position, const Vector &velocity, const floatType radius) const {
    const DynamicConvexPolygon poly = this->polyGen.generatePolygon(100, radius, position, 0.8f);
    spdlog::info("Rock spawned at {0}, {1}", position.x, position.y);
    DynamicProperties dynamicProperties{position, 0, 0, velocity, 1};
    return {position,         poly.getBodySpaceVertices(), poly.getTextureCoordinates(),
            poly.getMass(),   poly.getDensity(),           poly.getMomentOfInertia(),
            dynamicProperties};
}

void DevMode::spawnRocks(const std::vector<Rock> &rocks) {
    for (auto &rock : rocks) {
        this->world.addRock(std::make_shared<Rock>(rock));
    }
}

Vector DevMode::getTransformedPosition(const Vector &position) const {
    Vector transformedPos = Vector();
    transformedPos.x = position.x;
    transformedPos.y =
        GraphicsUtil::transformYCoordinate(camera.target.y) - position.y + static_cast<float>(GetScreenHeight()) / 2;
    return transformedPos;
}

void DevMode::renderText(std::string const &message) const {
    const char *text = message.c_str();
    const auto centerX = GetScreenWidth() - MeasureText(text, gameConstants.visualConstants.fontSizeScore);
    DrawText(text, centerX / 2, gameConstants.visualConstants.uiMargin * 1, gameConstants.visualConstants.fontSizeScore,
             RED);
}