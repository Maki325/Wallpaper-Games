#include "pch.h"
#include "Renderer.h"
#include <ft2build.h>
#include FT_FREETYPE_H

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
    MakeContextCurrent();
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

    m_shader.Use();

    InitText();
  }
  
  void Renderer::InitText()
  {
    m_textProjection = glm::ortho(0.0f, 1920.0f, 0.0f, 1080.0f);

    m_textShader.Create("resources/shaders/text/vertex.glsl", "resources/shaders/text/fragment.glsl");

    m_textShader.Use();
    m_textShader.LoadMatrix4f("projection", glm::value_ptr(m_textProjection));


    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
      throw std::runtime_error("ERROR::FREETYPE: Could not init FreeType Library");
    }

    FT_Face face;
    // "resources/fonts/WorkSans-VariableFont_wght.ttf"
    if (FT_New_Face(ft, "resources/fonts/WorkSans-Bold.ttf", 0, &face))
    {
      throw std::runtime_error("ERROR::FREETYPE: Failed to load font");
    }

    FT_Set_Pixel_Sizes(face, 0, 48);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

    // unsigned int textureIds[CHARACTER_COUNT];
    // glGenTextures(CHARACTER_COUNT, textureIds);
    for (unsigned char c = 0; c < CHARACTER_COUNT; c++)
    {
      // load character glyph 
      if (FT_Load_Char(face, c, FT_LOAD_RENDER))
      {
        std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
        continue;
      }

      unsigned int textureId = 0;
      glGenTextures(1, &textureId);

      // generate texture
      glBindTexture(GL_TEXTURE_2D, textureId);
      glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RED,
        face->glyph->bitmap.width,
        face->glyph->bitmap.rows,
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        face->glyph->bitmap.buffer
      );
      // set texture options
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      m_characters[c] = {
          textureId,
          glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
          glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
          (unsigned int) face->glyph->advance.x
      };
      std::cout << "m_characters[c].textureID: " << m_characters[c].textureID << std::endl;
    }
    
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    glGenVertexArrays(1, &m_textVAO);
    glGenBuffers(1, &m_textVBO);
    glBindVertexArray(m_textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // enable byte-alignment restriction
  }

  void Renderer::MakeContextCurrent()
  {
    wglMakeCurrent(m_windowDeviceContext, m_openGLRenderingContext);
  }

  Renderer::~Renderer()
  {
    wglDeleteContext(m_openGLRenderingContext);
    // for (auto& c : m_characters)
    // {
    //   glDeleteTextures(1, &c.textureID);
    // }
  }

  void Renderer::RenderEntity(Entity& entity)
  {
    m_shader.Use();
    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, entity.m_position);
    transform = glm::scale(transform, entity.m_scale);

    transform = glm::rotate(transform, glm::radians(entity.m_rotation.x), glm::vec3(1, 0, 0));
    transform = glm::rotate(transform, glm::radians(entity.m_rotation.y), glm::vec3(0, 1, 0));
    transform = glm::rotate(transform, glm::radians(entity.m_rotation.z), glm::vec3(0, 0, 1));

    m_shader.LoadMatrix4f("transform", glm::value_ptr(transform));

    GL_CHECK(glActiveTexture(GL_TEXTURE0));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, entity.m_texture.textureId));

    GL_CHECK(glBindVertexArray(entity.m_VAO));

    GL_CHECK(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));

    glBindVertexArray(0);
  }

  void Renderer::RenderLine(Line &line, glm::vec3 &position)
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

  void Renderer::RenderAABB(AABB& aabb)
  {
    auto &[position, size] = aabb;

    glm::vec2 topLeft     = glm::vec2(position.x - size.x, position.y + size.y);
    glm::vec2 topRight    = glm::vec2(position.x + size.x, position.y + size.y);
    glm::vec2 bottomLeft  = glm::vec2(position.x - size.x, position.y - size.y);
    glm::vec2 bottomRight = glm::vec2(position.x + size.x, position.y - size.y);

    RenderLine(Line(glm::vec3(topLeft,    0), glm::vec3(topRight,    0), glm::vec4(1, 0, 1, 1)));
    RenderLine(Line(glm::vec3(topLeft,    0), glm::vec3(bottomLeft,  0), glm::vec4(1, 0, 1, 1)));
    RenderLine(Line(glm::vec3(topRight,   0), glm::vec3(bottomRight, 0), glm::vec4(1, 0, 1, 1)));
    RenderLine(Line(glm::vec3(bottomLeft, 0), glm::vec3(bottomRight, 0), glm::vec4(1, 0, 1, 1)));
  }

  void Renderer::RenderText(const std::string& text, float x, float y, float scale, glm::vec3 color)
  {
    m_textShader.Use();
    GL_CHECK(glEnable(GL_CULL_FACE));
    GL_CHECK(glDisable(GL_DEPTH_TEST));
    GL_CHECK(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

    m_textShader.LoadVec3("textColor", color);
    GL_CHECK(glActiveTexture(GL_TEXTURE0));
    GL_CHECK(glBindVertexArray(m_textVAO));

    // iterate through all characters
    for (auto &c : text)
    {
      Character &ch = m_characters[c];

      float xpos = x + ch.bearing.x * scale;
      float ypos = y - (ch.size.y - ch.bearing.y) * scale;

      float w = ch.size.x * scale;
      float h = ch.size.y * scale;
      // update VBO for each character
      float vertices[6][4] = {
        { xpos,     ypos + h,   0.0f, 0.0f },
        { xpos,     ypos,       0.0f, 1.0f },
        { xpos + w, ypos,       1.0f, 1.0f },
        
        { xpos,     ypos + h,   0.0f, 0.0f },
        { xpos + w, ypos,       1.0f, 1.0f },
        { xpos + w, ypos + h,   1.0f, 0.0f }
      };
      // render glyph texture over quad
      GL_CHECK(glBindTexture(GL_TEXTURE_2D, ch.textureID));
      // update content of VBO memory
      GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_textVBO));
      GL_CHECK(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices));
      GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
      // render quad
      GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, 6));
      // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
      x += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
    GL_CHECK(glBindVertexArray(0));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
    GL_CHECK(glPixelStorei(GL_UNPACK_ALIGNMENT, 4));

    GL_CHECK(glEnable(GL_DEPTH_TEST));
    GL_CHECK(glDisable(GL_CULL_FACE));
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
    m_lineShader.Use();
    m_lineShader.LoadMatrix4f("projection", glm::value_ptr(m_projection));

    m_textProjection = glm::ortho(x, x + width, y, y + height);

    m_textShader.Use();
    m_textShader.LoadMatrix4f("projection", glm::value_ptr(m_textProjection));
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
