#pragma once
#include "Component.h"

namespace WallpaperAPI
{
  namespace SystemTray
  {
    class SyncedCheckbox : public Component
    {
    public:
      SyncedCheckbox(const std::string& text, bool &toggled);

      void Toggle();
      bool IsToggled();

      void CreateWindowsComponent(MENUITEMINFO& winItem) override;
      void HandleClick() override;

    private:
      bool& m_toggled;
    };
  }
}
