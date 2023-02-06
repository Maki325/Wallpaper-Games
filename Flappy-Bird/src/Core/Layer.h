#pragma once

namespace WallpaperAPI
{
  class Layer
  {
  public:
    Layer(const std::string& name = "Layer");

    virtual void OnAttach() = 0;
    virtual void OnDetach() = 0;

    virtual void OnUpdate(float delta) = 0;

    virtual void OnImGuiRender() = 0;

    const std::string& GetName() const { return m_debugName; }

    virtual const std::string& GetID() const = 0;
  public:
    struct Options {
      bool enabled;
    };
  protected:
    std::string m_debugName;
  };
}
