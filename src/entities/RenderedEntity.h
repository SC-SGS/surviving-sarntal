//
// Created by Anietta Weckauff on 07.05.24.
//

#ifndef SURVIVING_SARNTAL_RENDEREDENTITY_H
#define SURVIVING_SARNTAL_RENDEREDENTITY_H

#include "../components/vector.h"

class RenderedEntity {
  public:
    void setRenderInformation(RenderInformation &info);
    RenderInformation getRenderInformation();

    void setPosition(Vector &position);
    Vector getPosition();

  protected:
    RenderInformation renderInformation;
    Vector position;
};

#endif // SURVIVING_SARNTAL_RENDEREDENTITY_H
