#pragma once
#include "pch.h"

namespace WallpaperAPI
{
  struct Monitor
  {
    HMONITOR monitorId;
    RECT area;
    RECT workingArea;
  };
}
