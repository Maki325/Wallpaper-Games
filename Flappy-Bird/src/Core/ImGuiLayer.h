#pragma once
#include "Layer.h"
#include <GLFW/glfw3.h>

namespace WallpaperAPI
{
  class ImGuiLayer : public Layer
  {
  public:
    ImGuiLayer(HWND hwnd);

    void OnAttach() override;
    void OnDetach() override;
    bool ShouldDetach() override;

    void OnUpdate(float delta) override;
    void OnImGuiRender() override;

    void Begin();
    void End();

  private:
    HWND m_hwnd;
    bool m_show_demo_window = true;
  };
}


