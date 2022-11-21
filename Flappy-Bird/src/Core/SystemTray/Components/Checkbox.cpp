#include "pch.h"
#include "Checkbox.h"

namespace WallpaperAPI
{
  namespace SystemTray
  {
    Checkbox::Checkbox(const std::string& text, std::function<void(bool)> callback, bool toggled)
      : Component(text), m_callback(callback), m_toggled(toggled) {}

    void Checkbox::HandleClick() { if(m_callback) m_callback((m_toggled = !m_toggled)); }

    bool Checkbox::IsToggled() { return m_toggled; }

    void Checkbox::CreateWindowsComponent(MENUITEMINFO &winItem)
    {
      if(this->IsDisabled())
        winItem.fState = MFS_DISABLED;

      winItem.fState |= IsToggled() ? MFS_CHECKED : MFS_UNCHECKED;
    }
  }
}
