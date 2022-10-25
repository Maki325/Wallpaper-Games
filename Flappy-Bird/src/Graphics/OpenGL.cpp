#include "pch.h"
#include "OpenGL.h"

namespace WallpaperAPI
{
  OpenGL::OpenGL(HWND hWnd, HDC windowDeviceContext)
    : m_hWnd(hWnd), m_windowDeviceContext(windowDeviceContext)
  {
    Init();
  }

  void OpenGL::Init()
  {
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

    int letWindowsChooseThisPixelFormat = ChoosePixelFormat(m_windowDeviceContext, &pfd);
    SetPixelFormat(m_windowDeviceContext, letWindowsChooseThisPixelFormat, &pfd);

    m_openGLRenderingContext = wglCreateContext(m_windowDeviceContext);
    wglMakeCurrent(m_windowDeviceContext, m_openGLRenderingContext);

    m_initialized = gladLoadGL();
  }

  OpenGL::~OpenGL()
  {
    wglDeleteContext(m_openGLRenderingContext);
  }

  HWND OpenGL::GetHWnd()
  {
    return m_hWnd;
  }

  void OpenGL::SwapBuffers()
  {
    ::SwapBuffers(m_windowDeviceContext);
  }

  void OpenGL::SetViewport(RECT& rect)
  {
    glViewport(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
  }

  void OpenGL::SetViewport(int x, int y, int width, int height)
  {
    glViewport(x, y, width, height);
  }

}