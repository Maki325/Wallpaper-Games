#pragma once
#include "ShaderProgram.h"

namespace WallpaperAPI
{
  class Renderer
  {
  public:
    Renderer(HWND hWnd, HDC windowDeviceContext);
    ~Renderer();

    void CreateOpenGLContext();
    void Init();

    void SwapBuffers();
    void SetViewport(RECT& rect);
    void SetViewport(int x, int y, int width, int height);

    HWND GetHWnd();
    bool IsInitialized() { return m_initialized; }
    void SetMessageCallback();
  private:
    ShaderProgram m_shader;

    HWND m_hWnd;
    HGLRC m_openGLRenderingContext;
    HDC m_windowDeviceContext;
    bool m_initialized;
  };
}
