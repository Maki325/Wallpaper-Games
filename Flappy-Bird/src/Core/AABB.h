#pragma once

namespace WallpaperAPI
{
  class AABB
  {
  public:
    AABB(glm::vec2 position, glm::vec2 size);

    glm::vec2 m_position;
    glm::vec2 m_size;

    bool IsColliding(AABB& other);
    static bool IsColliding(AABB& one, AABB& two);
  };
}


