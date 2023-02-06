#include "pch.h"
#include "Text.h"
#include "Renderer.h"

namespace WallpaperAPI
{
  Text::Text(const std::string& text, int x, int y, glm::vec3& color)
    : m_text(text), m_x(x), m_y(y), m_color(color) {}

  Text::Text(const std::string&& text, int x, int y, glm::vec3& color)
    : m_text(std::move(text)), m_x(x), m_y(y), m_color(color) {}

  bool Text::ShouldRenderOverflow(int currentWidth, size_t charPosition, Renderer& renderer) const
  {
    char c = text().at(charPosition);
    Renderer::Character& ch = renderer.m_characters[c];
    int nextCharWidth = ch.advance * scale();
    switch (overflow())
    {
      case Text::Overflow::Ellipsis:
      {
        int dotWidth = renderer.m_characters['.'].advance * scale();
        return (currentWidth + nextCharWidth + dotWidth * 3) > width();
      }
      case Text::Overflow::Clip:
        return (currentWidth + nextCharWidth) > width();
    }
    return false;
  }

  void Text::RenderOverflow(Renderer& renderer, int x, int y, int maxY, size_t charPosition, int currentWidth) const
  {
    switch (overflow())
    {
      case Text::Overflow::Ellipsis:
      {
        int dotWidth = renderer.m_characters['.'].advance * scale();

        renderer.RenderCharacter('.', x + 0 * dotWidth, y, scale(), maxY);
        renderer.RenderCharacter('.', x + 1 * dotWidth, y, scale(), maxY);
        renderer.RenderCharacter('.', x + 2 * dotWidth, y, scale(), maxY);
        return;
      }
      case Text::Overflow::Clip:
      {
        int diff = width() - currentWidth;
        if (diff == 0) return;
        char c = text().at(charPosition);
        int charWidth = renderer.m_characters[c].advance * scale();
        float percent = diff * 1.0f / charWidth;

        // We totaly don't have a pixel over the edge problem, at all
        renderer.m_textShader.LoadVec2("cutoff", { percent, 0 });
        renderer.RenderCharacter(c, x, y, scale(), maxY);
        return;
      }
    }

    return;
  }
}