//
// Created by six on 9/23/24.
//

#include "EntityRenderer.h"

EntityRenderer::EntityRenderer(World &world,
                               Camera2D &camera,
                               GameConstants &gameConstants,
                               ResourceManager &resourceManager,
                               PolygonRenderer &polygonRenderer)
    : world(world),
      camera(camera),
      gameConstants(gameConstants),
      resourceManager(resourceManager),
      polygonRenderer(polygonRenderer) {}

// Function to render an entity
void EntityRenderer::renderEntity(const RenderedEntity &entity) const {
    renderEntity(entity, entity.getRenderInformation().rotationAngle);
}

void EntityRenderer::renderEntity(const RenderedEntity &entity, const floatType rotation) const {
    auto info = entity.getRenderInformation();

    const Texture2D texture = resourceManager.getTexture(info.texture);
    auto directedWidth = static_cast<floatType>(texture.width * (info.width >= 0 ? 1 : -1));
    // part of the texture used
    const Rectangle sourceRec = {0.0f, 0.0f, directedWidth, static_cast<floatType>(texture.height)};
    renderEntity(entity, rotation, texture, sourceRec);
}

void EntityRenderer::renderEntity(const RenderedEntity &entity,
                                  const floatType rotation,
                                  const Texture2D &texture,
                                  const Rectangle sourceRec) const {
    auto info = entity.getRenderInformation();
    info.width *= graphics::UNIT_TO_PIXEL_RATIO;
    info.height *= graphics::UNIT_TO_PIXEL_RATIO;

    // Define the destination rectangle
    const Vector transformedPosition = GraphicsUtil::transformPosition(info.position, info.offset);
    const Rectangle destRec = {transformedPosition.x, transformedPosition.y, std::abs(info.width), info.height};

    // Define the origin for rotation.
    const Vector2 origin = {std::abs(info.width) / 2, info.height / 2};

    // Draw the texture if not in debug mode
    if (!this->debugMode) {
        DrawTexturePro(texture, sourceRec, destRec, origin, rotation, WHITE);
    } else {
        // Undo the width flip if negative
        const int transformedWidth = static_cast<int>(info.width) * (info.width >= 0 ? 1 : -1);
        // Draw Rectangle for collision box, center with width and height
        DrawRectangleLines(static_cast<int>(destRec.x - destRec.width / 2),
                           static_cast<int>(destRec.y - destRec.height / 2), transformedWidth,
                           static_cast<int>(info.height), RED);
    }
}

void EntityRenderer::animateEntity(const RenderedEntity &entity) {
    auto info = entity.getRenderInformation();
    const auto currentTime = static_cast<floatType>(GetTime());

    AnimationInformation &animation = getAnimationInformation(entity.getId(), info.animation);

    // Check if we need to advance to the next frame
    if (currentTime >= animation.lastTime + animation.frameTime) {
        // Advance to the next frame
        animation.currentFrame = (animation.currentFrame + 1) % animation.frames;

        // Reset current time
        animation.lastTime = currentTime;
    }
    animations.insert_or_assign(entity.getId(), animation);

    renderAnimation(entity);
}

void EntityRenderer::renderAnimation(const RenderedEntity &entity) {
    // Get render and animation information
    auto info = entity.getRenderInformation();
    auto animation = getAnimationInformation(entity.getId(), info.animation);

    // Get the texture
    const Texture2D texture = resourceManager.getTexture(info.texture);

    // Calculate frame width based on the texture and animation frames
    const floatType frameWidth = static_cast<floatType>(texture.width) / static_cast<floatType>(info.animation.frames);
    const floatType width = frameWidth * static_cast<floatType>(info.width >= 0 ? 1 : -1);

    // Calculate current frame's X position in the texture
    const floatType currentFrameX = frameWidth * static_cast<floatType>(animation.currentFrame);

    // Flip horizontally if width is negative
    floatType directedFrameX = info.width >= 0 ? currentFrameX : currentFrameX + frameWidth;
    // Create source rectangle for the current frame
    Rectangle sourceRec = {directedFrameX, 0.0f, width, static_cast<floatType>(texture.height)};

    // Render the entity with the updated frame
    this->renderEntity(entity, info.rotationAngle, texture, sourceRec);
}

void EntityRenderer::renderHiker(const Hiker &hiker) {
    HikerMovement::MovementState state = hiker.getHikerMovement().getState();

    switch (state) {
    case HikerMovement::MovementState::IN_AIR:
        renderEntity(hiker);
        break;
    case HikerMovement::MovementState::MOVING:
        renderWalkingHiker(hiker);
        break;
    case HikerMovement::MovementState::CROUCHED:
        renderEntity(hiker);
        break;
    }
}

void EntityRenderer::renderWalkingHiker(const Hiker &hiker) {
    HikerMovement::Direction direction = hiker.getHikerMovement().getDirection();

    if (direction == HikerMovement::Direction::NEUTRAL) {
        // Get the texture
        const Texture2D texture = resourceManager.getTexture(hiker.getRenderInformation().texture);
        const floatType width = static_cast<floatType>(texture.width) /
                                static_cast<floatType>(hiker.getRenderInformation().animation.frames);
        Rectangle sourceRec = {0.0f, 0.0f, width, static_cast<floatType>(texture.height)};
        renderEntity(hiker, hiker.getRenderInformation().rotationAngle, texture, sourceRec);
    } else {
        animateEntity(hiker);
    }
}

void EntityRenderer::renderRock(const std::shared_ptr<Rock> &rock) const {
    polygonRenderer.renderTexturedPolygon(rock);
}

void EntityRenderer::debugRenderRock(const Rock &rock) const {
    polygonRenderer.renderPolygonOutline(rock);
    /*
    const auto transformedPosition = GraphicsUtil::transformPosition(entity.getRenderInformation().position);

    // Draw Circle for collision box
    DrawCircleLines(static_cast<int>(transformedPosition.x), static_cast<int>(transformedPosition.y),
                    (entity.getRenderInformation().width * graphics::UNIT_TO_PIXEL_RATIO) / 2, RED);

    // Calculate the end points of the line to detect rotation
    const floatType rotation = entity.getRenderInformation().angularOffset;
    const floatType radius = entity.getRenderInformation().width * graphics::UNIT_TO_PIXEL_RATIO / 2;
    const floatType rotation = entity.getRenderInformation().rotationAngle;
    const floatType radius = entity.getRenderInformation().width / 2;
    const int endX = static_cast<int>(transformedPosition.x + radius * std::cos(rotation));
    const int endY = static_cast<int>(transformedPosition.y + radius * std::sin(rotation));

    // Draw the line
    DrawLine(static_cast<int>(transformedPosition.x), static_cast<int>(transformedPosition.y), endX, endY, RED);
    */
}

void EntityRenderer::renderEntities(bool newDebugMode) {
    this->debugMode = newDebugMode;
    if (!debugMode)
        renderNormalEntities();
    else {
        debugRenderEntities();
    }
}

void EntityRenderer::addExplosion(const Rock &rock) const {
    // TODO the explosion needs to fit the bounding box
    Rock destroyedRock(rock.getPosition(), rock.getBodySpaceVertices(), rock.getTextureCoordinates(), 0.0f, 0.0f, 0.0f,
                       DynamicProperties());
    destroyedRock.setAnimationInformation({25, 0, 0.1, 0});
    this->destroyedRocks->push_back(destroyedRock);
}

void EntityRenderer::renderNormalEntities() {
    auto &hiker = world.getHiker();
    auto &rocks = world.getRocks();
    auto &monster = world.getMonster();

    // Render destroyed rocks, e.g. explosions
    for (auto &destroyedRock : getDestroyedRocks()) {
        animateEntity(destroyedRock);
    }

    // Render hiker
    renderHiker(hiker);

    // Render rocks
    for (auto &rock : rocks) {
        renderRock(rock);
    }

    // Render monster
    animateEntity(monster);

    // Render Items
    for (const auto &item : world.getItems()) {
        renderEntity(*item);
    }
}

void EntityRenderer::debugRenderEntities() {
    const auto &hiker = world.getHiker();
    const auto &rocks = world.getRocks();
    const auto &monster = world.getMonster();
    const auto &terrain = world.getTerrain();
    // Render hiker
    debugRenderHiker(hiker);
    // Render rocks
    for (auto &rock : rocks) {
        debugRenderRock(*rock);
    }
    // Render monster
    DrawLine(static_cast<int>(monster.getRenderInformation().position.x * graphics::UNIT_TO_PIXEL_RATIO),
             static_cast<int>(camera.target.y - (0.5 * GetScreenHeight())),
             static_cast<int>(monster.getRenderInformation().position.x * graphics::UNIT_TO_PIXEL_RATIO),
             GetScreenHeight(), RED);
    // Render Items
    for (const auto &item : world.getItems()) {
        const auto transformedPosition = GraphicsUtil::transformPosition(item->getRenderInformation().position);
        renderEntity(*item);
        constexpr auto FONT_SIZE = 20; // TODO magic number alert
        const auto itemType = item->getRenderInformation().texture.c_str();
        const auto centerX = static_cast<int>(transformedPosition.x) - MeasureText(itemType, FONT_SIZE) / 2;
        DrawText(itemType, centerX, static_cast<int>(transformedPosition.y), FONT_SIZE, GREEN);
        DrawCircleLines(static_cast<int>(transformedPosition.x), static_cast<int>(transformedPosition.y),
                        gameConstants.itemsConstants.collectionRadius * graphics::UNIT_TO_PIXEL_RATIO, BLUE);
    }
}

void EntityRenderer::debugRenderHiker(const Hiker &hiker) const {
    std::vector<Vector> vertices = hiker.getCurrentHitbox()->getWorldSpaceVertices();
    for (size_t i = 0; i < vertices.size(); ++i) {
        const Vector2 point1 = Vector2(GraphicsUtil::transformPosition(Vector2(vertices[i])));
        const Vector2 point2 = Vector2(GraphicsUtil::transformPosition(Vector2(vertices[(i + 1) % vertices.size()])));
        DrawLineEx({point1.x, point1.y}, {point2.x, point2.y}, 3, RED);
    }
}

void EntityRenderer::reset() {
    animations.clear();
    destroyedRocks->clear();
}

AnimationInformation &EntityRenderer::getAnimationInformation(int entityId, AnimationInformation &defaultAnimation) {
    if (animations.find(entityId) == animations.end()) {
        animations[entityId] = defaultAnimation;
    }
    return animations[entityId];
}

std::list<Rock> &EntityRenderer::getDestroyedRocks() const {
    destroyedRocks->remove_if([this](Rock &rock) {
        auto animation = animations.find(rock.getId());
        if (animation != animations.end()) {
            return animation->second.currentFrame >= animation->second.frames - 1;
        } else {
            return false;
        }
    });
    return *destroyedRocks;
}
