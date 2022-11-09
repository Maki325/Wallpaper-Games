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
    if ((one.Top() < two.Top() && one.Top() > two.Bottom()) || (one.Bottom() < two.Top() && one.Bottom() > two.Bottom()))
    {
      if ((one.Left() < two.Right() && one.Left() > two.Left()) || (one.Right() < two.Top() && one.Right() > two.Bottom()))
      {
        return true;
      }
    }
    return false;
  }

  float AABB::Top()    { return m_position.y + m_size.y; }
  float AABB::Bottom() { return m_position.y - m_size.y; }
  float AABB::Right()  { return m_position.x + m_size.x; }
  float AABB::Left()   { return m_position.x - m_size.x; }
}
