#pragma once
#include "Core/Layer.h"

namespace WallpaperAPI
{
  class ImGuiLayer : public Layer
  {
  public:
    ImGuiLayer(const std::string& debugName);

    void OnUpdate(float delta) override;
    void OnImGuiRender() override;

    virtual void Begin() = 0;
    virtual void End() = 0;
  };
}
