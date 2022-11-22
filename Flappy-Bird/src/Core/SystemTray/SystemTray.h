#pragma once
#include <Windows.h>
#include "Components/Component.h"

// Based on taypp: https://github.com/Soundux/traypp
namespace WallpaperAPI
{
  namespace SystemTray
  {
    class SystemTray
    {
    public:
      SystemTray(const std::string& identifier, const std::string& icon);
      ~SystemTray();

      void Update();

      template<typename T, typename std::enable_if<std::is_base_of<Component, T>::value>::type * = nullptr>
      void AddComponent(const T& component)
      {
        m_components.push_back(std::make_unique<T>(component));
        m_components.back()->SetSystemTray(this);
        UpdateTray();
      }

      void AddMenuOpenListener(std::function<void()>* listener);
      void RemoveMenuOpenListener(std::function<void()>* listener);

    private:
      void Init();
      void UpdateTray();
      void ShouldRecreateMenu();

      HMENU CreateMenu();
      static LRESULT CALLBACK wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

      std::vector<std::unique_ptr<Component>> m_components;
      std::vector<std::wstring> m_wideStrings;
      std::vector<std::function<void()>*> m_menuOpenedListeners;

      static std::unordered_map<HWND, SystemTray&> s_systemTrays;

      std::wstring m_identifier;
      HWND m_hWnd = nullptr;
      HMENU m_hMenu = nullptr;
      WNDCLASSEX m_windowClass;
      NOTIFYICONDATA m_notifyIconData;
      HICON m_hIcon;
    };
  }
}
