#pragma once
#include "Component.h"

namespace WallpaperAPI
{
  namespace SystemTray
  {
    class Checkbox : public Component
    {
    public:
      Checkbox(const std::string& text, std::function<void(bool)> callback, bool toggled = false);

      void Toggle();
      bool IsToggled();

      void CreateWindowsComponent(MENUITEMINFO& winItem) override;
      void HandleClick() override;

    private:
      bool m_toggled;
      std::function<void(bool)> m_callback;
    };
  }
}
