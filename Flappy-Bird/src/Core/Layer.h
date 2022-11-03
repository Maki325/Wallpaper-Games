#pragma once

namespace WallpaperAPI
{
  class Layer
  {
  public:
    Layer(const std::string& name = "Layer");

    virtual void OnAttach() = 0;
    virtual void OnDetach() = 0;
    virtual bool ShouldDetach() { return false; }

    virtual void OnUpdate(float delta) = 0;

    virtual void OnImGuiRender() = 0;

    const std::string& GetName() const { return m_debugName; }
  protected:
    std::string m_debugName;
  };
}
