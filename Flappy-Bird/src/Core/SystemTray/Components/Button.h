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

    private:
      std::function<void()> m_callback;
    };
  }
}
