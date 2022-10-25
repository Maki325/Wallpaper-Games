#include "pch.h"
#include "MonitorManager.h"
#include "../Utils/Utils.h"
#include "Core.h"

namespace WallpaperAPI
{

  MonitorManager::MonitorManager(HWND desktopHWnd, HDC desktopDC)
    : m_desktopHWnd(desktopHWnd), m_desktopDC(desktopDC)
  {
  }

  std::vector<Monitor>& MonitorManager::GetMonitors()
  {
    if (!s_monitors.empty()) {
      return s_monitors;
    }

    RECT windowSize;
    GetWindowRect(m_desktopHWnd, &windowSize);
    Utils::NormalizeRect(windowSize);

    EnumDisplayMonitors(
      m_desktopDC,
      &windowSize,
      EnumerateMonitors,
      0
    );

    int maxMinusLeft = 0, maxMinusTop = 0;
    for (auto& monitor : s_monitors) {
      if (monitor.area.left < 0 && monitor.area.left < maxMinusLeft) maxMinusLeft = monitor.area.left;
      if (monitor.area.top < 0 && monitor.area.top < maxMinusTop) maxMinusTop = monitor.area.top;

      HBRUSH brush = CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256)); //create brush
      SelectObject(m_desktopDC, brush); //select brush into DC
      Rectangle(m_desktopDC, monitor.area.left, monitor.area.top, monitor.area.right, monitor.area.bottom); //draw rectangle over whole screen
    }

    for (auto& monitor : s_monitors) {
      monitor.area.left -= maxMinusLeft;
      monitor.area.right -= maxMinusLeft;
      monitor.area.top -= maxMinusTop;
      monitor.area.bottom -= maxMinusTop;
    }

    return s_monitors;
  }

  BOOL MonitorManager::EnumerateMonitors(HMONITOR hMonitor, HDC hDc, LPRECT pRect, LPARAM data) {
    if (!hMonitor) {
      INFO("No monitor!");
      return true;
    }
    MONITORINFO monitorInfo;
    monitorInfo.cbSize = sizeof(MONITORINFO);
    GetMonitorInfo(hMonitor, &monitorInfo);

    s_monitors.push_back({
      hMonitor,
      (monitorInfo.rcMonitor),
      (monitorInfo.rcWork) }
    );

    return true;
  }

  std::vector<Monitor> MonitorManager::s_monitors;
}
