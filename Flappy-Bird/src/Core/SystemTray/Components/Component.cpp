#include "pch.h"
#include "Component.h"
#include "../SystemTray.h"

namespace WallpaperAPI
{
  namespace SystemTray
  {
    Component::Component(const std::string &text)
      : m_text(text) {}

    std::string& Component::GetText() { return m_text; }

    void Component::SetDisabled(bool disabled) { m_disabled = disabled; }
    bool Component::IsDisabled() { return m_disabled; }

    void Component::SetSystemTray(SystemTray* systemTray) {
      m_systemTray = systemTray;
    }
  }
}
