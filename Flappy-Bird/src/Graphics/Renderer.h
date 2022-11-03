#pragma once
#include "ShaderProgram.h"
#include "../Entity/Line.h"
#include "../Entity/Entity.h"

namespace WallpaperAPI
{
  class Renderer
  {
  public:
    Renderer(HWND hWnd, HDC windowDeviceContext);
    ~Renderer();

    void CreateOpenGLContext();
    void Init();

    void MakeContextCurrent();

    void RenderEntity(Entity& entity);
    void RenderLine(Line& line, glm::vec3 position);

    void SwapBuffers();
    void SetViewport(RECT& rect);
    void SetViewport(int x, int y, int width, int height);

    HWND GetHWnd();
    bool IsInitialized() { return m_initialized; }
    void SetMessageCallback();

    ShaderProgram& GetShaderProgram();

  public:
    glm::mat4 m_projection = glm::mat4(1.0f);
    glm::mat4 m_view = glm::mat4(1.0f);

  private:
    ShaderProgram m_shader;
    ShaderProgram m_lineShader;


    HWND m_hWnd;
    HGLRC m_openGLRenderingContext;
    HDC m_windowDeviceContext;
    bool m_initialized;
  };
}
