#include "pch.h"
#include "AABB.h"

namespace WallpaperAPI
{
  AABB::AABB(glm::vec2 position, glm::vec2 size)
    : Collider(), m_position(position), m_size(size) {}

  float AABB::Top()    { return m_position.y + m_size.y; }
  float AABB::Bottom() { return m_position.y - m_size.y; }
  float AABB::Right()  { return m_position.x + m_size.x; }
  float AABB::Left()   { return m_position.x - m_size.x; }
}
