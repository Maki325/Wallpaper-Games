#pragma once

namespace WallpaperAPI
{
  class Texture
  {
  public:
    Texture(const char* location);
    void Init();
    void Clean();

    const char* location;
    unsigned int textureId;
    int width, height, colorChannels;
  };
}

