#pragma once

namespace WallpaperAPI
{
  struct Renderer;

  struct Text
  {
    Text(const std::string& text, int x, int y, glm::vec3& m_color = glm::vec3(1, 1, 1));
    Text(const std::string&& text, int x, int y, glm::vec3& color);

    const std::string& text() const { return this->m_text; }

    Text& x(float x) { this->m_x = x; return *this; }
    float x() const { return this->m_x; }

    Text& y(float y) { this->m_y = y; return *this; }
    float y() const { return this->m_y; }

    Text& color(glm::vec3& color) { this->m_color = color; return *this; }
    glm::vec3& color() const { return this->m_color; }

    Text &scale(float scale) { this->m_scale = scale; return *this; }
    float scale() const { return this->m_scale; }

    Text& centerHorizontal() { this->m_centeredHorizontal = true; return *this; }
    Text& centeredHorizontal(bool centered) { this->m_centeredHorizontal = centered; return *this; }
    bool centeredHorizontal() const { return this->m_centeredHorizontal; }

    Text& centerVertical() { this->m_centeredVertically = true; return *this; }
    Text& centeredVertical(bool centered) { this->m_centeredVertically = centered; return *this; }
    bool centeredVertical() const { return this->m_centeredVertically; }

    Text& shadow(bool shadow) { this->m_shadow = shadow; return *this; }
    Text& shadow(glm::vec3& shadowColor) { this->m_shadow = true; this->m_shadowColor = shadowColor; return *this; }
    bool shadow() const { return this->m_shadow; }

    Text& shadowColor(glm::vec3& shadowColor) { this->m_shadowColor = shadowColor; return *this; }
    glm::vec3& shadowColor() const { return this->m_shadowColor; }

    Text& width(int width) { this->m_width = width; return *this; }
    int width() const { return this->m_width; }

    enum Overflow;
    Text& overflow(Overflow overflow) { this->m_overflow = overflow; return *this; }
    Overflow overflow() const { return this->m_overflow; }

    bool ShouldRenderOverflow(int currentWidth, size_t charPosition, Renderer &renderer) const;
    void RenderOverflow(Renderer& renderer, int x, int y, int maxY, size_t charPosition, int currentWidth) const;
  public:
    enum Overflow {
      Ellipsis = 0,
      Clip,
    };

  private:
    const std::string& m_text;
    float m_x = 0;
    float m_y = 0;
    glm::vec3& m_color;
    float m_scale = 1.0f;
    bool m_centeredHorizontal = false;
    bool m_centeredVertically = false;
    bool m_shadow = false;
    glm::vec3& m_shadowColor = glm::vec3(0.0f, 0.0f, 0.0f);
    int m_width = 0;
    Overflow m_overflow = Overflow::Ellipsis;
  };
}
