#pragma once
#include "ShaderProgram.h"
#include "Entity/Line.h"
#include "Entity/Entity.h"
#include "Colliders/AABB.h"
#include "pch.h"

namespace WallpaperAPI
{
  class Renderer
  {
  public:
    Renderer(HWND hWnd, HDC windowDeviceContext);
    ~Renderer();

    void CreateOpenGLContext();
    void Init();
    void InitText();

    void MakeContextCurrent();

    void RenderEntity(Entity& entity);
    void RenderLine(Line& line, glm::vec3 &position = glm::vec3(0));
    void RenderAABB(AABB& aabb);

    void RenderText(const std::string& text, float x, float y, glm::vec3 &color, float scale = 1.0f, bool centered = false, bool shadow = false, glm::vec3& shadowColor = glm::vec3(0.0f, 0.0f, 0.0f));
    int GetTextWidth(const std::string& text, float scale = 1.0f);
    int GetTextHeight(const std::string& text, float scale = 1.0f);

    void RenderColoredQuad(float x, float y, float width, float height, glm::vec4& color);
    void RenderTexturedQuad(float x, float y, float width, float height, Texture& texture, int textureX = 0, int textureY = 0, int textureWidth = 0, int textureHeight = 0);

    void RenderButton(float x, float y, float width, float height, Texture& background, const std::string& text, glm::vec3& textColor, float textScale = 1.0f, int textureX = 0, int textureY = 0, int textureWidth = 0, int textureHeight = 0);

    void SwapBuffers();
    void SetViewport(RECT& rect);
    void SetViewport(int x, int y, int width, int height);
    glm::vec4 GetViewport();

    HWND GetHWnd();
    bool IsInitialized() { return m_initialized; }
    void SetMessageCallback();

    ShaderProgram& GetShaderProgram();
  private:
    void RenderCharacter(char c, float x, float y, float scale, int maxY);

  public:
    glm::mat4 m_projection = glm::mat4(1.0f);
    glm::mat4 m_view = glm::mat4(1.0f);

    glm::mat4 m_orthoProjection = glm::mat4(1.0f);

    glm::vec4 m_viewport = glm::vec4(0);

  private:
    struct Character {
      unsigned int textureID;  // ID handle of the glyph texture
      glm::ivec2   size;       // Size of glyph
      glm::ivec2   bearing;    // Offset from baseline to left/top of glyph
      glm::ivec2   hang;
      unsigned int advance;    // Offset to advance to next glyph
    };

  private:
    ShaderProgram m_shader;
    ShaderProgram m_lineShader;
    ShaderProgram m_textShader;
    ShaderProgram m_uiShader;

    HWND m_hWnd;
    HGLRC m_openGLRenderingContext;
    HDC m_windowDeviceContext;
    bool m_initialized;

    static const size_t CHARACTER_COUNT = 128;

    Character m_characters[CHARACTER_COUNT];
    unsigned int m_textVAO, m_textVBO;
  };
}
