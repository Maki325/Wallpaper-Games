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

  class Texture
  {
  public:
    Texture(const char *location);
    const char *location;
    unsigned int textureId;
    int width, height, colorChannels;
  };

  class Entity
  {
  public:
    Entity(Model model, glm::vec3 position, glm::vec3 velocity, glm::vec3 rotation, const char *texture);
    ~Entity();

  public:
    void GenerateTexture();
    void GenerateData();

    Model m_model;
    glm::vec3 m_position;
    glm::vec3 m_velocity;
    glm::vec3 m_rotation;

    unsigned int m_VBO, m_VAO, m_EBO;

    Texture m_texture;
  };

}
