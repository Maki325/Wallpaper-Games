#include "pch.h"
#include "Entity.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../Graphics/stb_image.h"

namespace WallpaperAPI
{
  Texture::Texture(const char *location)
    : textureId(0), width(0), height(0), colorChannels(0), location(location) {}

  Entity::Entity(Model model, glm::vec3 position, glm::vec3 velocity, glm::vec3 rotation, const char* texture)
    : m_model(model), m_position(position), m_velocity(velocity), m_rotation(rotation), m_texture(Texture(texture))
  {
    Entity::GenerateData();
    Entity::GenerateTexture();
  }

  Entity::~Entity()
  {
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);

    glDeleteTextures(1, &m_texture.textureId);
  }

  void Entity::GenerateData()
  {
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_model.vertices.size() * sizeof(Vertex), m_model.vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_model.indices.size() * sizeof(glm::uvec3), m_model.indices.data(), GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
  }

  void Entity::GenerateTexture()
  {
    glGenTextures(1, &m_texture.textureId);
    glBindTexture(GL_TEXTURE_2D, m_texture.textureId);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char* data = stbi_load(m_texture.location, &m_texture.width, &m_texture.height, &m_texture.colorChannels, 0);
    if (data)
    {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_texture.width, m_texture.height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
      throw std::runtime_error("Failed to load texture");
    }
    stbi_image_free(data);
  }
}
