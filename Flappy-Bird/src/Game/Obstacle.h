#pragma once
#include "Entity/Entity.h"
#include "Graphics/Renderer.h"
#include "Colliders/CircleCollider.h"

namespace WallpaperAPI
{
  class Obstacle
  {
  public:
    Obstacle();

    void Render(Renderer& renderer);

    bool IsColliding(CircleCollider& aabb);

    void SetPosition(glm::vec3& position);
    glm::vec3& GetPosition();
  public:
    glm::vec3 m_position;
    Entity m_top, m_bottom;

    AABB m_topAABB, m_bottomAABB;
  };

}
