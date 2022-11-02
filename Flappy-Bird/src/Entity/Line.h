#pragma once

namespace WallpaperAPI
{
  class Line
  {
  public:
    Line(glm::vec3 start, glm::vec3 end, glm::vec4 color);

  public:
    glm::vec3 m_vertices[2];
    glm::vec4 m_color;
    unsigned int m_VAO = 0, m_VBO = 0;
  };
}
