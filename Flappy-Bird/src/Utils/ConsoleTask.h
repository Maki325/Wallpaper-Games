#pragma once
#include "StoppableTask.h"

namespace WallpaperAPI
{
  class ConsoleTask: public StoppableTask
  {
  public:
    ConsoleTask();
    void Run() override;

    static void InitConsole();
  };
}
