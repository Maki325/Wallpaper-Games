#include "pch.h"
#include "CircleCollider.h"

namespace WallpaperAPI
{
  CircleCollider::CircleCollider(glm::vec2 position, float radius)
    : Collider(), m_position(position), m_radius(radius)
  {
  }
}
