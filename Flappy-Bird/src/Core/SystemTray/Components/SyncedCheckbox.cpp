#include "pch.h"
#include "SyncedCheckbox.h"

namespace WallpaperAPI
{
  namespace SystemTray
  {
    SyncedCheckbox::SyncedCheckbox(const std::string& text, bool &toggled)
      : Component(text), m_toggled(toggled) {}

    void SyncedCheckbox::HandleClick() { m_toggled = !m_toggled; }

    bool SyncedCheckbox::IsToggled() { return m_toggled; }

    void SyncedCheckbox::CreateWindowsComponent(MENUITEMINFO &winItem)
    {
      if(this->IsDisabled())
        winItem.fState = MFS_DISABLED;

      winItem.fState |= IsToggled() ? MFS_CHECKED : MFS_UNCHECKED;
    }

    bool SyncedCheckbox::ShouldRecreate(MENUITEMINFO& winItem) {
      if ((winItem.fState & MFS_CHECKED) == winItem.fState && IsToggled()
        || (winItem.fState & MFS_UNCHECKED) == winItem.fState && !IsToggled())
      {
        return false;
      }
      return true;
    }
  }
}
