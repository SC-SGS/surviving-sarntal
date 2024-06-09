//
// Created by Anietta Weckauff on 07.05.24.
//

#ifndef SURVIVING_SARNTAL_RENDEREDENTITY_H
#define SURVIVING_SARNTAL_RENDEREDENTITY_H

#include "../output/graphics/renderInformation/RenderInformation.h"
#include "../utilities/vector.h"

/**
 * This class serves as a base class for rendered entities.
 * Hiker, Item, Monster and Rock inherit from this class.
 * Key features of a rendered item are position and necessary render information.
 */
class RenderedEntity {
  public:
    explicit RenderedEntity(Vector position);

    virtual RenderInformation getRenderInformation();
    void setAnimationInformation(AnimationInformation &animationInformation);

    void setPosition(Vector &position);
    const Vector &getPosition() const;

  protected:
    Vector position{};
    AnimationInformation animation{0, 0, 0, 0};
};

#endif // SURVIVING_SARNTAL_RENDEREDENTITY_H
