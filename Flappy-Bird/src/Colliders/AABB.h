#pragma once
#include "Collider.h"

namespace WallpaperAPI
{
  class AABB : public Collider
  {
  public:
    AABB(glm::vec2 position, glm::vec2 size);

    float Top();
    float Bottom();
    float Left();
    float Right();
  public:
    glm::vec2 m_position;
    glm::vec2 m_size;
  };
}


