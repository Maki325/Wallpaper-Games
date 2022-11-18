#include "pch.h"
#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../Graphics/stb_image.h"

namespace WallpaperAPI
{
  Texture::Texture(const char* location)
    : textureId(0), width(0), height(0), colorChannels(0), location(location)
  {
    Init();
  }

  void Texture::Init()
  {
    GL_CHECK(glGenTextures(1, &textureId));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, textureId));
    // set the texture wrapping parameters
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));	// set texture wrapping to GL_REPEAT (default wrapping method)
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
    // set texture filtering parameters
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    // load image, create texture and generate mipmaps
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char* data = stbi_load(location, &width, &height, &colorChannels, STBI_rgb_alpha);

    if (data)
    {
      GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
      GL_CHECK(glGenerateMipmap(GL_TEXTURE_2D));
    }
    else
    {
      throw std::runtime_error("Failed to load texture");
    }
    stbi_image_free(data);
  }

  void Texture::Clean()
  {
    GL_CHECK(glDeleteTextures(1, &textureId));
  }
}
