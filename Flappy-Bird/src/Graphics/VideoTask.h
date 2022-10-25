#pragma once
#include "../Utils/StoppableTask.h"
#include "OpenGL.h"

namespace WallpaperAPI
{
  class VideoTask : public StoppableTask
  {
  public:
    VideoTask(std::string&& videoPath);
    VideoTask(std::string&& videoPath, double seconds);
    void Run() override;

  private:
    std::string m_videoPath;
    double m_seconds;
  };

  class VideoFrame {
  public:
    int width;
    int height;
    unsigned char* data;
  };
}

