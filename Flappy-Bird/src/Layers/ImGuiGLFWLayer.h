#pragma once
#include "ImGuiLayer.h"
#include <GLFW/glfw3.h>

namespace WallpaperAPI
{
  class ImGuiGLFWLayer : public ImGuiLayer
  {
  public:
    ImGuiGLFWLayer();

    void OnAttach() override;
    void OnDetach() override;

    void Begin() override;
    void End() override;

    const std::string& GetID() const override { return ID; }
    static const std::string& ID;
  private:
    GLFWwindow* m_window = nullptr;
  };
}


