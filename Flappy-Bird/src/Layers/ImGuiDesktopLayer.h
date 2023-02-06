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

    const std::string& GetID() const override { return ID; }
    static const std::string ID;
  private:
    HWND m_hwnd;
  };
}


