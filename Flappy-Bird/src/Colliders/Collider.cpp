#include <pch.h>
#include "Collider.h"
#include "AABB.h"
#include "CircleCollider.h"

namespace WallpaperAPI
{
  Collider::Collider()
  {
  }

  template<>
  bool Collider::IsColliding<AABB, AABB>(AABB& one, AABB& two)
  {
    if ((one.Top() < two.Top() && one.Top() > two.Bottom()) || (one.Bottom() < two.Top() && one.Bottom() > two.Bottom()))
    {
      if ((one.Left() < two.Right() && one.Left() > two.Left()) || (one.Right() < two.Right() && one.Right() > two.Left()))
      {
        return true;
      }
    }
    return false;
  }

  template<>
  bool Collider::IsColliding<CircleCollider, AABB>(CircleCollider& one, AABB& two)
  {
    float distX = abs(one.m_position.x - two.m_position.x);
    float distY = abs(one.m_position.y - two.m_position.y);

    if (distX > one.m_radius + two.m_size.x)
    {
      return false;
    }
    if (distY > one.m_radius + two.m_size.y)
    {
      return false;
    }

    if (distX <= one.m_radius + two.m_size.x && distY <= one.m_radius + two.m_size.y)
    {
      return true;
    }

    float dx = (distX - two.m_size.x);
    float dy = (distY - two.m_size.y);
    float cornerDistance = dx * dx + dy * dy;

    return (cornerDistance <= (one.m_radius * one.m_radius));
  }

}
