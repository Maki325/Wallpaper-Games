#include "pch.h"
#include "Entity.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../Graphics/stb_image.h"

namespace WallpaperAPI
{
  Texture::Texture(const char *location)
    : textureId(0), width(0), height(0), colorChannels(0), location(location) {}

  Entity::Entity(Model model, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::vec3 velocity, const char* texture)
    : m_model(model), m_position(position), m_rotation(rotation), m_scale(scale), m_velocity(velocity), m_texture(Texture(texture))
  {
    Entity::GenerateData();
    Entity::GenerateTexture();
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
    Entity::GenerateTexture();
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
      GL_CHECK(glDeleteTextures(1, &this->m_texture.textureId));
    }

    this->m_model.vertices = std::vector(entity.m_model.vertices);
    this->m_model.indices = std::vector(entity.m_model.indices);
    this->m_position = entity.m_position;
    this->m_rotation = entity.m_rotation;
    this->m_scale = entity.m_scale;
    this->m_velocity = entity.m_velocity;
    this->m_texture.location = entity.m_texture.location;

    Entity::GenerateData();
    Entity::GenerateTexture();

    return *this;
  }

  Entity::~Entity()
  {
    GL_CHECK(glDeleteVertexArrays(1, &m_VAO));
    GL_CHECK(glDeleteBuffers(1, &m_VBO));
    GL_CHECK(glDeleteBuffers(1, &m_EBO));

    GL_CHECK(glDeleteTextures(1, &m_texture.textureId));
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

  void Entity::GenerateTexture()
  {
    GL_CHECK(glGenTextures(1, &m_texture.textureId));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_texture.textureId));
    // set the texture wrapping parameters
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));	// set texture wrapping to GL_REPEAT (default wrapping method)
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
    // set texture filtering parameters
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    // load image, create texture and generate mipmaps
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char* data = stbi_load(m_texture.location, &m_texture.width, &m_texture.height, &m_texture.colorChannels, STBI_rgb_alpha);

    std::cout << "data: " << (void *) data << std::endl;

    if (data)
    {
      GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_texture.width, m_texture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
      GL_CHECK(glGenerateMipmap(GL_TEXTURE_2D));
    }
    else
    {
      throw std::runtime_error("Failed to load texture");
    }
    stbi_image_free(data);
  }

  std::ostream& operator<<(std::ostream& out, Vertex& v)
  {
    out << "{\"position\": {\"x\": " << v.position.x <<
      ", \"y\": " << v.position.y <<
      ", \"z\": " << v.position.z <<
      "}, \"texture\": {\"x\": " << v.texture.x <<
      ", \"y\": " << v.texture.y << "}";

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
      out << "v: {\"x\": " << index.x << ", \"y\": " << index.y << ", \"z\": " << index.z << "}";
      if (i != model.indices.size() - 1) {
        out << ",";
      }
    }
    out << "]}";
    return out;
  }
}
