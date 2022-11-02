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
    GL_CHECK(glEnable(GL_DEPTH_TEST));
    GL_CHECK(glEnable(GL_DEBUG_OUTPUT));

    GL_CHECK(glEnable(GL_BLEND));
    GL_CHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    m_projection = glm::perspective(glm::radians(45.0f), 1920.0f / 1080.0f, 0.01f, 1000.0f);
    m_view = glm::translate(m_view, glm::vec3(0.0f, 0.0f, -3.0f));

    m_lineShader.Create("resources/shaders/line/vertex.glsl", "resources/shaders/line/fragment.glsl");

    m_shader.Create("resources/shaders/main/vertex.glsl", "resources/shaders/main/fragment.glsl");
    m_shader.Use();

    m_shader.Use();
    m_shader.LoadMatrix4f("projection", glm::value_ptr(m_projection));
    m_shader.LoadMatrix4f("view", glm::value_ptr(m_view));

    m_lineShader.Use();
    m_lineShader.LoadMatrix4f("projection", glm::value_ptr(m_projection));
    m_lineShader.LoadMatrix4f("view", glm::value_ptr(m_view));

    glm::mat4 inverse = glm::mat4(0);
    inverse = m_projection * m_view;
    inverse = glm::inverse(inverse);

    glm::vec4 in = glm::vec4(0.1, 0.1, 0, 1);
    glm::vec4 out = inverse * in;

    std::cout << "Out: " << out.x << ", " << out.y << ", " << out.z << ", " << out.w << std::endl;
    std::cout << "Out: " << out.x * out.w << ", " << out.y * out.w << ", " << out.z * out.w << ", " << out.w << std::endl;

    m_shader.Use();
  }

  Renderer::~Renderer()
  {
    wglDeleteContext(m_openGLRenderingContext);
  }

  void Renderer::RenderLine(Line &line, glm::vec3 position)
  {
    GL_CHECK(glDisable(GL_DEPTH_TEST));
    m_lineShader.Use();

    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, position);

    m_lineShader.LoadMatrix4f("transform", glm::value_ptr(transform));
    m_lineShader.LoadVec4("color", line.m_color);

    glBindVertexArray(line.m_VAO);
    glDrawArrays(GL_LINES, 0, 2);
    glBindVertexArray(0);
    GL_CHECK(glEnable(GL_DEPTH_TEST));
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
    Renderer::SetViewport(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
  }

  void Renderer::SetViewport(int x, int y, int width, int height)
  {
    GL_CHECK(glViewport(x, y, width, height));

    m_projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.01f, 1000.0f);

    m_shader.Use();
    m_shader.LoadMatrix4f("projection", glm::value_ptr(m_projection));
  }

  void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
  {
    std::cout << "GL CALLBACK: " << (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "") << " type = " << type << ", severity = " << severity << ", message = " << message << std::endl;
  }

  void Renderer::SetMessageCallback()
  {
    GL_CHECK(glDebugMessageCallback(MessageCallback, 0));
  }
}
