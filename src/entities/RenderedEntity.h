//
// Created by Anietta Weckauff on 07.05.24.
//

#ifndef SURVIVING_SARNTAL_RENDEREDENTITY_H
#define SURVIVING_SARNTAL_RENDEREDENTITY_H

// TODO remove this include
#include "../components/vector.h"
#include "../graphics/render_information/RenderInformation.h"

/**
 * This class serves as a base class for rendered entities.
 * Hiker, Item, Monster and Rock inherit from this class.
 * Key features of a rendered item are position and necessary render information.
 */
class RenderedEntity {
  public:
    explicit RenderedEntity(Vector position);

    virtual RenderInformation getRenderInformation();

    // TODO why do we use their vector? we need our own and also, renamings like Position as vector alias for clarity
    void setPosition(Vector &position);
    Vector getPosition();

  protected:
    Vector position{};
};

#endif // SURVIVING_SARNTAL_RENDEREDENTITY_H
