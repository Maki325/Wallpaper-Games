#include <pch.h>
#include "Line.h"

namespace WallpaperAPI
{
  Line::Line(glm::vec3 start, glm::vec3 end, glm::vec4 color)
    : m_color(color)
  {
    m_vertices[0] = start;
    m_vertices[1] = end;

    GL_CHECK(glGenVertexArrays(1, &m_VAO));
    GL_CHECK(glGenBuffers(1, &m_VBO));

    GL_CHECK(glBindVertexArray(m_VAO));

    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(glm::vec3), &m_vertices[0], GL_STATIC_DRAW));

    GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));
    GL_CHECK(glEnableVertexAttribArray(0));

    GL_CHECK(glBindVertexArray(0));
  }

  Line::~Line()
  {
    Destroy();
  }

  void Line::Destroy()
  {
    GL_CHECK(glDeleteVertexArrays(1, &m_VAO));
    GL_CHECK(glDeleteBuffers(1, &m_VBO));
  }
}
