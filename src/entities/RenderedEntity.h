//
// Created by Anietta Weckauff on 07.05.24.
//

#ifndef SURVIVING_SARNTAL_RENDEREDENTITY_H
#define SURVIVING_SARNTAL_RENDEREDENTITY_H

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

    void setPosition(Vector &position);
    Vector getPosition();

  protected:
    Vector position{};
};

#endif // SURVIVING_SARNTAL_RENDEREDENTITY_H
