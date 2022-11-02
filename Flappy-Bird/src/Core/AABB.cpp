#include "pch.h"
#include "AABB.h"

namespace WallpaperAPI
{
  AABB::AABB(glm::vec2 position, glm::vec2 size)
    : m_position(position), m_size(size) {}

  bool AABB::IsColliding(AABB& other)
  {
    return IsColliding(*this, other);
  }
  bool AABB::IsColliding(AABB& one, AABB& two)
  {
    bool collisionX = one.m_position.x + one.m_size.x / 2 >= two.m_position.x - two.m_size.x / 2 &&
      two.m_position.x + two.m_size.x / 2 >= one.m_position.x - one.m_size.x / 2;
    // collision y-axis?
    bool collisionY = one.m_position.y + one.m_size.y / 2 >= two.m_position.y - two.m_size.y / 2 &&
      two.m_position.y + two.m_size.y / 2 >= one.m_position.y - one.m_size.y / 2;
    // collision only if on both axes
    return collisionX && collisionY;
  }
}
