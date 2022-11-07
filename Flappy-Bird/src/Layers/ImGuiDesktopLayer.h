#pragma once
#include "ImGuiLayer.h"

namespace WallpaperAPI
{
  class ImGuiDesktopLayer : public ImGuiLayer
  {
  public:
    ImGuiDesktopLayer(HWND hwnd);

    void OnAttach() override;
    void OnDetach() override;

    void Begin() override;
    void End() override;

  private:
    HWND m_hwnd;
  };
}


