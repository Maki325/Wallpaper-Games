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

      bool IsToggled();

      void CreateWindowsComponent(MENUITEMINFO& winItem) override;
      void HandleClick() override;

      bool ShouldRecreate(MENUITEMINFO& winItem) override;
    private:
      bool& m_toggled;
    };
  }
}
