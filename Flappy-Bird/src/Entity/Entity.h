#pragma once
#include <glm/glm.hpp>
#include "Graphics/Texture.h"

namespace WallpaperAPI
{
  struct Vertex
  {
    glm::vec3 position;
    glm::vec2 texture;

    friend std::ostream& operator<<(std::ostream& out, Vertex& robject);
  };
  struct Model
  {
    std::vector<Vertex> vertices;
    std::vector<glm::uvec3> indices;

    friend std::ostream& operator<<(std::ostream& out, Model& robject);
  };

  class Entity
  {
  public:
    Entity(Model model, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::vec3 velocity, const char *texture);
    Entity(Entity& entity);
    ~Entity();

  public:
    void GenerateData();

    Entity& operator= (const Entity& str);

    Model m_model;
    glm::vec3 m_position;
    glm::vec3 m_rotation;
    glm::vec3 m_scale;
    glm::vec3 m_velocity;

    unsigned int m_VBO = 0, m_VAO = 0, m_EBO = 0;

    Texture m_texture;

  private:
    void Clean();
  };

}
