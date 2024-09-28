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
    static int idCounter;

  public:
    virtual ~RenderedEntity() = default;
    explicit RenderedEntity(Vector position);

    virtual RenderInformation getRenderInformation() const = 0;
    void setAnimationInformation(AnimationInformation animationInformation);

    virtual void setPosition(const Vector &position);
    const Vector &getPosition() const;

    const int &getId() const { return id; }

  protected:
    int id;
    Vector position{};
    AnimationInformation animation{0, 0, 0, 0};
};

#endif // SURVIVING_SARNTAL_RENDEREDENTITY_H
