#pragma once
#include "Monitor.h"

namespace WallpaperAPI
{
  class MonitorManager
  {
  public:
    MonitorManager(HWND desktopHWnd, HDC desktopDC);
    std::vector<Monitor>& GetMonitors();
  private:
    HWND m_desktopHWnd;
    HDC m_desktopDC;

    static std::vector<Monitor> s_monitors;

    static BOOL EnumerateMonitors(HMONITOR hMonitor, HDC hDc, LPRECT pRect, LPARAM data);
  };
}

