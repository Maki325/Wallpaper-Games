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

  LineFormula GetLineFormula(glm::vec2 p1, glm::vec2 p2)
  {
    float y = p2.y - p1.y;
    float x = p2.x - p1.x;
    std::cout << "X: " << x << std::endl;
    float m = x == 0 ? 0 : (y / x);
    std::cout << "mmmmm: " << m << std::endl;
    float b = p2.y - (m * p2.x);
    return { m, b };
  }

  float DistanceBetweenLineAndPoint(glm::vec2 p1, glm::vec2 p2, glm::vec2 point)
  {
    if (p1.x == p2.x)
    {
      std::cout << "S" << std::endl;
      return abs(point.x - p1.x);
    }
    auto formula = GetLineFormula(p1, p2);
    // y = mx + c
    // y = -(A / B) * x - C / B
    // 
    // c = C / B
    // m = -(A / B)
    //  
    std::cout << "formula.m: " << formula.m << std::endl;
    std::cout << "formula.b: " << formula.b << std::endl;
    return abs(formula.m * point.x + point.y + formula.b) / sqrt(formula.m * formula.m + 1);
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

    return false;
  }

}
