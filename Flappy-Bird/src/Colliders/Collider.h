#pragma once
#include <type_traits>

namespace WallpaperAPI
{
  struct LineFormula {
    float m;
    float b;
  };

  LineFormula GetLineFormula(glm::vec2 p1, glm::vec2 p2);
  float DistanceBetweenLineAndPoint(glm::vec2 p1, glm::vec2 p2, glm::vec2 point);

  class Collider
  {
  public:
    Collider();
  public:
    template<typename A, typename B, typename std::enable_if<std::is_base_of<Collider, A>::value>::type* = nullptr, typename std::enable_if<std::is_base_of<Collider, B>::value>::type* = nullptr>
    static bool IsColliding(A& one, B& two);
  };
}


