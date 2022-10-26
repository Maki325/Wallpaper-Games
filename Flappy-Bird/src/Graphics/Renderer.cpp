#include "pch.h"
#include "Renderer.h"

namespace WallpaperAPI
{
  Renderer::Renderer(HWND hWnd, HDC windowDeviceContext)
    : m_hWnd(hWnd), m_windowDeviceContext(windowDeviceContext)
  {
    Init();
  }

  void Renderer::CreateOpenGLContext()
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
  }

  void Renderer::Init()
  {
    CreateOpenGLContext();
    if (!(m_initialized = gladLoadGL())) {
      throw std::exception("Couldn't load Glad!");
    }
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_DEBUG_OUTPUT);

    m_shader.Create("resources/shaders/vertex.glsl", "resources/shaders/fragment.glsl");
    m_shader.Use();
  }

  Renderer::~Renderer()
  {
    wglDeleteContext(m_openGLRenderingContext);
  }

  HWND Renderer::GetHWnd()
  {
    return m_hWnd;
  }

  ShaderProgram& Renderer::GetShaderProgram()
  {
    return m_shader;
  }

  void Renderer::SwapBuffers()
  {
    ::SwapBuffers(m_windowDeviceContext);
  }

  void Renderer::SetViewport(RECT& rect)
  {
    glViewport(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
  }

  void Renderer::SetViewport(int x, int y, int width, int height)
  {
    glViewport(x, y, width, height);
  }

  void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
    std::cout << "GL CALLBACK: " << (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "") << " type = " << type << ", severity = " << severity << ", message = " << message << std::endl;
  }

  void Renderer::SetMessageCallback() {
    glDebugMessageCallback(MessageCallback, 0);
  }
}
