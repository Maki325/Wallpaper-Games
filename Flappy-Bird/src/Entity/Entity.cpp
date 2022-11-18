#include "pch.h"
#include "Entity.h"

namespace WallpaperAPI
{

  Entity::Entity(Model model, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::vec3 velocity, const char* texture)
    : m_model(model), m_position(position), m_rotation(rotation), m_scale(scale), m_velocity(velocity), m_texture(Texture(texture))
  {
    Entity::GenerateData();
  }

  Entity::Entity(Entity& entity)
    :m_texture(Texture(entity.m_texture.location))
  {
    this->m_model.vertices = std::vector(entity.m_model.vertices);
    this->m_model.indices = std::vector(entity.m_model.indices);
    this->m_position = entity.m_position;
    this->m_rotation = entity.m_rotation;
    this->m_scale = entity.m_scale;
    this->m_velocity = entity.m_velocity;

    Entity::GenerateData();
  }

  Entity& Entity::operator= (const Entity& entity)
  {
    if (this->m_VAO)
    {
      GL_CHECK(glDeleteVertexArrays(1, &this->m_VAO));
    }
    if (this->m_VBO)
    {
      GL_CHECK(glDeleteBuffers(1, &this->m_VBO));
    }
    if (this->m_EBO)
    {
      GL_CHECK(glDeleteBuffers(1, &this->m_EBO));
    }
    if (this->m_texture.textureId)
    {
      this->m_texture.Clean();
    }

    this->m_model.vertices = std::vector(entity.m_model.vertices);
    this->m_model.indices = std::vector(entity.m_model.indices);
    this->m_position = entity.m_position;
    this->m_rotation = entity.m_rotation;
    this->m_scale = entity.m_scale;
    this->m_velocity = entity.m_velocity;
    this->m_texture.location = entity.m_texture.location;

    Entity::GenerateData();
    this->m_texture.Init();

    return *this;
  }

  Entity::~Entity()
  {
    Clean();
  }

  void Entity::Clean()
  {
    GL_CHECK(glDeleteVertexArrays(1, &m_VAO));
    GL_CHECK(glDeleteBuffers(1, &m_VBO));
    GL_CHECK(glDeleteBuffers(1, &m_EBO));

    m_texture.Clean();
  }

  void Entity::GenerateData()
  {
    GL_CHECK(glGenVertexArrays(1, &m_VAO));
    GL_CHECK(glGenBuffers(1, &m_VBO));
    GL_CHECK(glGenBuffers(1, &m_EBO));

    GL_CHECK(glBindVertexArray(m_VAO));

    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, m_model.vertices.size() * sizeof(Vertex), m_model.vertices.data(), GL_STATIC_DRAW));

    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO));
    GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_model.indices.size() * sizeof(glm::uvec3), m_model.indices.data(), GL_STATIC_DRAW));

    // position attribute
    GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0));
    GL_CHECK(glEnableVertexAttribArray(0));
    // texture coord attribute
    GL_CHECK(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))));
    GL_CHECK(glEnableVertexAttribArray(1));

    GL_CHECK(glBindVertexArray(0));
  }

  std::ostream& operator<<(std::ostream& out, Vertex& v)
  {
    out << "{\"position\": {\"x\": " << v.position.x <<
      ", \"y\": " << v.position.y <<
      ", \"z\": " << v.position.z <<
      "}, \"texture\": {\"x\": " << v.texture.x <<
      ", \"y\": " << v.texture.y << "}}";

    return out;
  }

  std::ostream& operator<<(std::ostream& out, Model& model)
  {
    out << "Model: {\"vertices\": [";
    for (size_t i = 0; i < model.vertices.size(); i++) {
      out << model.vertices.at(i);
      if (i != model.vertices.size() - 1) {
        out << ",";
      }
    }
    out << "], \"indices\": [";
    for (size_t i = 0; i < model.indices.size(); i++) {
      glm::uvec3& index = model.indices.at(i);
      out << "{\"x\": " << index.x << ", \"y\": " << index.y << ", \"z\": " << index.z << "}";
      if (i != model.indices.size() - 1) {
        out << ",";
      }
    }
    out << "]}";
    return out;
  }
}
