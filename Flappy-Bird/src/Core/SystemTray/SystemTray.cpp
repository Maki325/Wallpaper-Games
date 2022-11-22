#include "pch.h"
#include "SystemTray.h"
#include "Components/Button.h"
#include "Core/Application.h"
#include <codecvt>

static constexpr auto WM_SYS_TRAY = WM_APP + 1;

namespace WallpaperAPI
{
  namespace SystemTray
  {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

    SystemTray::SystemTray(const std::string& identifier, const std::string& icon)
      : m_identifier(std::move(converter.from_bytes(identifier))),
      m_hIcon((HICON)LoadImageA(nullptr, icon.c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE))
    {
      Init();
      UpdateTray();
    }

    void SystemTray::Init()
    {
      memset(&m_windowClass, 0, sizeof(m_windowClass));
      m_windowClass.cbSize = sizeof(m_windowClass);
      m_windowClass.lpfnWndProc = wndProc;
      m_windowClass.lpszClassName = this->m_identifier.c_str();
      m_windowClass.hInstance = GetModuleHandle(nullptr);

      if (!RegisterClassEx(&m_windowClass))
      {
        throw std::runtime_error("Couldn't register window class for the system tray!");
      }

      m_hWnd = CreateWindow(this->m_identifier.c_str(), nullptr, 0, 0, 0, 0, 0, nullptr, nullptr, m_windowClass.hInstance, nullptr);

      if (m_hWnd == nullptr)
      {
        throw std::runtime_error("Failed to create a window for the system tray!");
      }

      s_systemTrays.emplace(m_hWnd, *this);

      if (UpdateWindow(m_hWnd) == 0)
      {
        throw std::runtime_error("Failed to update window used for the system tray!");
      }

      memset(&m_notifyIconData, 0, sizeof(NOTIFYICONDATA));
      m_notifyIconData.cbSize = sizeof(NOTIFYICONDATA);
      m_notifyIconData.hWnd = m_hWnd;
      m_notifyIconData.uFlags = NIF_ICON | NIF_MESSAGE;
      m_notifyIconData.uCallbackMessage = WM_SYS_TRAY;
      m_notifyIconData.hIcon = this->m_hIcon;

      if (Shell_NotifyIcon(NIM_ADD, &m_notifyIconData) == FALSE)
      {
        throw std::runtime_error("Failed to register system tray icon!");
      }
    }

    SystemTray::~SystemTray()
    {
      Shell_NotifyIcon(NIM_DELETE, &m_notifyIconData);
      DestroyIcon(m_notifyIconData.hIcon);
      DestroyMenu(m_hMenu);

      UnregisterClass(m_identifier.c_str(), GetModuleHandle(nullptr));
      PostMessage(m_hWnd, WM_QUIT, 0, 0);

      s_systemTrays.erase(m_hWnd);
    }

    void SystemTray::UpdateTray()
    {
      DestroyMenu(m_hMenu);
      m_hMenu = CreateMenu();

      if (Shell_NotifyIcon(NIM_MODIFY, &m_notifyIconData) == FALSE)
      {
        throw std::runtime_error("Failed to update tray icon");
      }
      SendMessage(m_hWnd, WM_INITMENUPOPUP, reinterpret_cast<WPARAM>(m_hMenu), 0);
    }

    HMENU SystemTray::CreateMenu()
    {
      static auto id = 0;
      m_wideStrings.clear();

      HMENU menu = CreatePopupMenu();
      for (auto it = m_components.rbegin(); it != m_components.rend(); it++)
      {
        auto* item = (*it).get();
        m_wideStrings.push_back(std::move(converter.from_bytes(item->GetText())));

        MENUITEMINFO winItem{ 0 };
        winItem.cbSize = sizeof(MENUITEMINFO);

        winItem.wID = ++id;
        winItem.dwTypeData = (wchar_t*) m_wideStrings.back().c_str();
        winItem.fMask = MIIM_TYPE | MIIM_STATE | MIIM_DATA | MIIM_ID;
        winItem.dwItemData = reinterpret_cast<ULONG_PTR>(item);

        item->CreateWindowsComponent(winItem);

        InsertMenuItem(menu, id, TRUE, &winItem);
      }

      return menu;
    }

    void SystemTray::ShouldRecreateMenu()
    {
      size_t count = GetMenuItemCount(m_hMenu);
      for (size_t i = 0; i < count; i++)
      {
        MENUITEMINFO item{ 0 };
        item.cbSize = sizeof(MENUITEMINFO);
        item.fMask = MIIM_TYPE | MIIM_STATE | MIIM_DATA | MIIM_ID;
        if (!GetMenuItemInfo(m_hMenu, i, true, &item)) {
          continue;
        }

        auto* component = reinterpret_cast<Component*>(item.dwItemData);
        bool should = component->ShouldRecreate(item);
        if (should)
        {
          UpdateTray();
          return;
        }
      }
    }

    void SystemTray::Update()
    {
      static MSG msg;
      while (PeekMessage(&msg, m_hWnd, 0, 0, PM_REMOVE))
      {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }
    }

    void SystemTray::AddMenuOpenListener(std::function<void()>* listener)
    {
      m_menuOpenedListeners.push_back(listener);
    }

    void SystemTray::RemoveMenuOpenListener(std::function<void()>* listener)
    {
      for (auto it = m_menuOpenedListeners.begin(); it != m_menuOpenedListeners.end(); it++)
      {
        if (*it == listener)
        {
          m_menuOpenedListeners.erase(it);
          return;
        }
      }
    }

    LRESULT CALLBACK SystemTray::wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
      switch (msg)
      {
      case WM_SYS_TRAY: {
        if (lParam != WM_RBUTTONUP) break;
        POINT p;
        GetCursorPos(&p);
        SetForegroundWindow(hwnd);
        SystemTray& tray = s_systemTrays.at(hwnd);
        for (auto* listener : tray.m_menuOpenedListeners) (*listener)();
        tray.ShouldRecreateMenu();
        auto cmd = TrackPopupMenu(tray.m_hMenu, TPM_RETURNCMD | TPM_NONOTIFY, p.x, p.y, 0, hwnd, nullptr);
        SendMessage(hwnd, WM_COMMAND, cmd, 0);
        return 0;
      }
      case WM_COMMAND: {
        MENUITEMINFO winItem{ 0 };
        winItem.fMask = MIIM_DATA | MIIM_ID;
        winItem.cbSize = sizeof(MENUITEMINFO);

        SystemTray & tray = s_systemTrays.at(hwnd);
        if (GetMenuItemInfo(tray.m_hMenu, static_cast<UINT>(wParam), FALSE, &winItem))
        {
          auto* item = reinterpret_cast<Component*>(winItem.dwItemData);
          item->HandleClick();
        }
        break;
      }
      }
      return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    std::unordered_map<HWND, SystemTray&> SystemTray::s_systemTrays;
  }
}
