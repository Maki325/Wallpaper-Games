#pragma once
#include "Collider.h"

namespace WallpaperAPI
{
  class CircleCollider : public Collider
  {
  public:
    CircleCollider(glm::vec2 position, float radius);

  public:
    glm::vec2 m_position;
    float m_radius;
  };
}


