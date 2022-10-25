#pragma once

namespace WallpaperAPI
{
  class OpenGL
  {
  public:
    OpenGL(HWND hWnd, HDC windowDeviceContext);
    ~OpenGL();

    void Init();

    void SwapBuffers();
    void SetViewport(RECT& rect);
    void SetViewport(int x, int y, int width, int height);

    HWND GetHWnd();
    bool IsInitialized() { return m_initialized; }
  private:
    HWND m_hWnd;
    HGLRC m_openGLRenderingContext;
    HDC m_windowDeviceContext;
    bool m_initialized;
  };
}

