#include <pch.h>
#include "Obstacle.h"
#include "Core/Loader.h"

namespace WallpaperAPI
{
  const float offset = 1.1f;
  const glm::vec2 hitboxSize(0.100f, 5.615f);

  Obstacle::Obstacle()
    : m_position(0, 0, 0),
    m_top(Loader::LoadObj("resources/models/pipe.obj"), glm::vec3(0, offset, 0), glm::vec3(180, 0, 0), glm::vec3(0.25), glm::vec3(0), "resources/textures/pipe.png"),
    m_bottom(Loader::LoadObj("resources/models/pipe.obj"), glm::vec3(0, -offset, 0), glm::vec3(0, 0, 0), glm::vec3(0.25), glm::vec3(0), "resources/textures/pipe.png"),
    m_topAABB(glm::vec2(0, offset + 5), hitboxSize),
    m_bottomAABB(glm::vec2(0, -offset - 5), hitboxSize)
  {
  }

  void Obstacle::Render(Renderer& renderer)
  {
    renderer.RenderEntity(m_top);
    renderer.RenderEntity(m_bottom);

    #ifndef WG_DIST
      renderer.RenderAABB(m_topAABB);
      renderer.RenderAABB(m_bottomAABB);
    #endif
  }

  void Obstacle::SetPosition(glm::vec3& position)
  {
    m_top.m_position.x = m_bottom.m_position.x = position.x;
    m_top.m_position.y = position.y + offset;
    m_bottom.m_position.y = position.y - offset;

    m_topAABB.m_position.x = m_bottomAABB.m_position.x = position.x;
    m_topAABB.m_position.y = position.y + offset + 5;
    m_bottomAABB.m_position.y = position.y - offset - 5;

    m_position = position;
  }
  glm::vec3& Obstacle::GetPosition()
  {
    return m_position;
  }

  bool Obstacle::IsColliding(CircleCollider& aabb)
  {
    return Collider::IsColliding(aabb, m_bottomAABB) || Collider::IsColliding(aabb, m_topAABB);
  }
}
