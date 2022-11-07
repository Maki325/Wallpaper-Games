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
    void InitConsole();
    std::chrono::milliseconds GetMillis();
    bool StartsWith(const std::string& string, const std::string& start, size_t beginning = 0);
    std::vector<std::string> Split(const std::string& str, const std::string& splitter);
  };
}
