#include "pch.h"
#include "Button.h"

namespace WallpaperAPI
{
  namespace SystemTray
  {
    Button::Button(const std::string& text, std::function<void()> callback)
      : Component(text), m_callback(callback) {}

    void Button::HandleClick() { if(m_callback) m_callback(); }

    void Button::CreateWindowsComponent(MENUITEMINFO &winItem)
    {
      if(this->IsDisabled())
        winItem.fState = MFS_DISABLED;
    }
  }
}
