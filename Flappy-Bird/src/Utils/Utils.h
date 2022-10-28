#pragma once

namespace WallpaperAPI
{
  namespace Utils
  {
    BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);
    HWND GetWallpaperWindow();
    void PrintRect(RECT r);
    void PrintWindowSize(HWND hWnd);
    RECT& NormalizeRect(RECT& rect);
    std::chrono::milliseconds GetMillis();
  };
}
