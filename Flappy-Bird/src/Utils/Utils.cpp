#include "pch.h"
#include "Utils.h"
#include <CommCtrl.h>

// LRESULT WINAPI WndProc22(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
LRESULT WINAPI WndProc23(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
  std::cout << "T" << std::endl;
  return 0;
}

namespace WallpaperAPI
{
  namespace Utils
  {
    BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
      HWND p = FindWindowEx(hwnd, NULL, L"SHELLDLL_DefView", NULL);
      HWND* ret = (HWND*)lParam;

      if (p)
      {
        // Gets the WorkerW Window after the current one.
        // SetWindowSubclass(hwnd, WndProc23, 14564654, 0);
        *ret = FindWindowEx(NULL, hwnd, L"WorkerW", NULL);
      }
      return true;
    }

    HWND GetWallpaperWindow() {
      // Fetch the Progman window
      HWND progman = FindWindow(L"ProgMan", NULL);
      // Send 0x052C to Progman. This message directs Progman to spawn a 
      // WorkerW behind the desktop icons. If it is already there, nothing 
      // happens.
      SendMessageTimeout(progman, 0x052C, 0, 0, SMTO_NORMAL, 1000, nullptr);
      // We enumerate all Windows, until we find one, that has the SHELLDLL_DefView 
      // as a child. 
      // If we found that window, we take its next sibling and assign it to workerw.
      HWND wallpaper_hwnd = nullptr;
      EnumWindows(EnumWindowsProc, (LPARAM)&wallpaper_hwnd);
      // SetWindowLongPtr(wallpaper_hwnd, GWLP_WNDPROC, (LONG_PTR)WndProc22);
      // Return the handle you're looking for.
      return wallpaper_hwnd;
    }

    void PrintRect(RECT r) {
      std::cout << "RECT: {" << r.left << ", " << r.top << ", " << r.right << ", " << r.bottom << "}" << std::endl;
    }

    void PrintWindowSize(HWND hWnd) {
      RECT hwndSize;
      GetWindowRect(hWnd, &hwndSize);
      PrintRect(hwndSize);
    }
  
    RECT& NormalizeRect(RECT& rect) {
      if (rect.left < 0) {
        rect.right -= rect.left;
        rect.left = 0;
      }
      if (rect.top < 0) {
        rect.bottom -= rect.top;
        rect.top = 0;
      }

      return rect;
    }

    // https://stackoverflow.com/a/46050762
    void InitConsole()
    {
      static const WORD MAX_CONSOLE_LINES = 500;

      auto stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);

      AllocConsole();

      // Get STDOUT handle
      HANDLE ConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
      int SystemOutput = _open_osfhandle(intptr_t(ConsoleOutput), _O_TEXT);
      FILE* COutputHandle = _fdopen(SystemOutput, "w");

      // Get STDERR handle
      HANDLE ConsoleError = GetStdHandle(STD_ERROR_HANDLE);
      int SystemError = _open_osfhandle(intptr_t(ConsoleError), _O_TEXT);
      FILE* CErrorHandle = _fdopen(SystemError, "w");

      // Get STDIN handle
      HANDLE ConsoleInput = GetStdHandle(STD_INPUT_HANDLE);
      int SystemInput = _open_osfhandle(intptr_t(ConsoleInput), _O_TEXT);
      FILE* CInputHandle = _fdopen(SystemInput, "r");

      //make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog point to console as well
      std::ios::sync_with_stdio(true);

      // Redirect the CRT standard input, output, and error handles to the console
      freopen_s(&CInputHandle, "CONIN$", "r", stdin);
      freopen_s(&COutputHandle, "CONOUT$", "w", stdout);
      freopen_s(&CErrorHandle, "CONOUT$", "w", stderr);

      //Clear the error state for each of the C++ standard stream objects. We need to do this, as
      //attempts to access the standard streams before they refer to a valid target will cause the
      //iostream objects to enter an error state. In versions of Visual Studio after 2005, this seems
      //to always occur during startup regardless of whether anything has been read from or written to
      //the console or not.
      std::wcout.clear();
      std::cout.clear();
      std::wcerr.clear();
      std::cerr.clear();
      std::wcin.clear();
      std::cin.clear();
    }
  
    std::chrono::milliseconds GetMillis()
    {
      return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
    }

    bool StartsWith(const std::string& string, const std::string& start, size_t beginning = 0)
    {
      if (start.length() > string.length()) return false;

      for (size_t i = 0; i < start.length(); i++)
      {
        if (string[i + beginning] != start[i]) return false;
      }
      return true;
    }

    std::vector<std::string> Split(const std::string& str, const std::string& splitter)
    {
      std::vector<std::string> splits;

      size_t last = 0;
      for (size_t i = 0; i < str.length(); i++)
      {
        if (StartsWith(str, splitter, i))
        {
          if (last == 0)
          {
            splits.push_back(str.substr(0, i - last));
          }
          else
          {
            splits.push_back(str.substr(last + 1, i - last - 1));
          }
          last = i;
        }
      }
      if (last == 0)
      {
        splits.push_back(str);
      }
      else {
        splits.push_back(str.substr(last + 1, str.size()));
      }
      return splits;
    }
  }
}
