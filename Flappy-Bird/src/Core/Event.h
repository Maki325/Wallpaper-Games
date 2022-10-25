#pragma once

namespace WallpaperAPI
{
  class Event
  {
  public:
    enum Type
    {
      EVENT_TYPE_QUIT = 0,
      EVENT_TYPE_RESET,
      EVENT_TYPE_RENDER,
      EVENT_TYPE_COUNT
    };

    Type type;
    void* data;
  };
}
