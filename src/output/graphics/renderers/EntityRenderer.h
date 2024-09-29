//
// Created by six on 9/23/24.
//

#ifndef SURVIVING_SARNTAL_ENTITYRENDERER_H
#define SURVIVING_SARNTAL_ENTITYRENDERER_H

#include "../../../entities/World.h"
#include "../GraphicsUtil.h"
#include "PolygonRenderer.h"
#include "raylib.h"
#include "rlgl.h"

class EntityRenderer {
  public:
    explicit EntityRenderer(World &world,
                            Camera2D &camera,
                            GameConstants &gameConstants,
                            ResourceManager &resourceManager,
                            PolygonRenderer &polygonRenderer);

    void renderEntities(bool debugMode);
    void reset();
    void addExplosion(const Rock &rock) const;

  private:
    World &world;
    Camera2D &camera;
    GameConstants &gameConstants;
    ResourceManager &resourceManager;

    bool debugMode = false;
    std::unordered_map<int, AnimationInformation> animations;
    const std::unique_ptr<std::list<Rock>> destroyedRocks = std::make_unique<std::list<Rock>>();
    PolygonRenderer &polygonRenderer;

    void debugRenderEntities();
    void renderNormalEntities();

    void renderEntity(const RenderedEntity &entity) const;
    void renderEntity(const RenderedEntity &entity, floatType rotation) const;
    void
    renderEntity(const RenderedEntity &entity, floatType rotation, const Texture2D &texture, Rectangle sourceRec) const;
    void renderRock(const std::shared_ptr<Rock> &rock) const;
    void renderHiker(const Hiker &hiker);
    void debugRenderHiker(const Hiker &hiker) const;
    void animateEntity(const RenderedEntity &entity);
    void renderAnimation(const RenderedEntity &entity);
    AnimationInformation &getAnimationInformation(int entityId, AnimationInformation &defaultAnimation);
    void debugRenderRock(const Rock &rock) const;
    void renderWalkingHiker(const Hiker &hiker);
    std::list<Rock> &getDestroyedRocks() const;
};

#endif // SURVIVING_SARNTAL_ENTITYRENDERER_H
