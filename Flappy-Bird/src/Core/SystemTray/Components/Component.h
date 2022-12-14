#pragma once

namespace WallpaperAPI
{
  namespace SystemTray
  {
    class SystemTray;

    class Component
    {
    public:
      Component(const std::string& text);

      std::string& GetText();

      void SetDisabled(bool disabled);
      bool IsDisabled();

      void SetSystemTray(SystemTray* systemTray);

      virtual void CreateWindowsComponent(MENUITEMINFO &winItem) = 0;
      virtual void HandleClick() = 0;
      virtual bool ShouldRecreate(MENUITEMINFO& winItem) = 0;

    protected:
      SystemTray *m_systemTray = nullptr;
      std::string m_text;
      bool m_disabled = false;
    };
  }
}
