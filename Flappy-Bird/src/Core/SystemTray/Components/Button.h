#pragma once
#include "Component.h"

namespace WallpaperAPI
{
  namespace SystemTray
  {
    class Button : public Component
    {
    public:
      Button(const std::string& text, std::function<void()> callback);

      void CreateWindowsComponent(MENUITEMINFO& winItem) override;
      void HandleClick() override;

      bool ShouldRecreate(MENUITEMINFO& winItem) override { return false; }
    private:
      std::function<void()> m_callback;
    };
  }
}
