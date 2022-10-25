#include "pch.h"
#include "Utils.h"

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
      // Return the handle you're looking for.
      return wallpaper_hwnd;
    }

    void PrintRect(RECT r) {
      std::cout << "RECT: {" << r.left << ", " << r.top << ", " << r.right << ", " << r.bottom << "}" << std::endl;
    }

    bool InitOpenGL(HWND hWnd) {
      // https://www.khronos.org/opengl/wiki/Creating_an_OpenGL_Context_%28WGL%29
      PIXELFORMATDESCRIPTOR pfd =
      {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    //Flags
        PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
        32,                   // Colordepth of the framebuffer.
        0, 0, 0, 0, 0, 0,
        0,
        0,
        0,
        0, 0, 0, 0,
        24,                   // Number of bits for the depthbuffer
        8,                    // Number of bits for the stencilbuffer
        0,                    // Number of Aux buffers in the framebuffer.
        PFD_MAIN_PLANE,
        0,
        0, 0, 0
      };

      HDC ourWindowHandleToDeviceContext = GetDC(hWnd);

      int  letWindowsChooseThisPixelFormat;
      letWindowsChooseThisPixelFormat = ChoosePixelFormat(ourWindowHandleToDeviceContext, &pfd);
      SetPixelFormat(ourWindowHandleToDeviceContext, letWindowsChooseThisPixelFormat, &pfd);

      HGLRC ourOpenGLRenderingContext = wglCreateContext(ourWindowHandleToDeviceContext);
      wglMakeCurrent(ourWindowHandleToDeviceContext, ourOpenGLRenderingContext);

      return gladLoadGL();
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
  }
}
