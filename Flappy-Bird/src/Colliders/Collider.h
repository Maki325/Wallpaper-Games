#pragma once
#include <type_traits>

namespace WallpaperAPI
{
  struct LineFormula {
    float m;
    float b;
  };

  class Collider
  {
  public:
    Collider();
  public:
    template<typename A, typename B, typename std::enable_if<std::is_base_of<Collider, A>::value>::type* = nullptr, typename std::enable_if<std::is_base_of<Collider, B>::value>::type* = nullptr>
    static bool IsColliding(A& one, B& two);
  };
}


