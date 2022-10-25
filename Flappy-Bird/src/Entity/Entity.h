#pragma once
#include <glm/glm.hpp>

namespace WallpaperAPI
{
  struct Vertex
  {
    glm::vec3 position;
    glm::vec2 texture;
  };
  struct Model
  {
    std::vector<Vertex> vertices;
    std::vector<glm::uvec3> indices;
  };

  class Entity
  {
    Model model;
    glm::vec3 position;
    glm::vec3 velocity;
    unsigned int m_textureId;
  };

}
