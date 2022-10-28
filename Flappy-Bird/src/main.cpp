#include "pch.h"
#include "Core/Application.h"
#include "Core/InputManager.h"

#define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))

static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  std::cout << "WindowProc" << std::endl;
  WallpaperAPI::InputManager *inputManager = (WallpaperAPI::InputManager*) GetPropW(hWnd, L"InputManager");
  if (!inputManager) {
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
  }

  switch (uMsg)
  {
  case WM_CLOSE:
  {
    // TODO: Add later maybe?
    // _glfwInputWindowCloseRequest(window);
    // return 0;
  }

  case WM_INPUTLANGCHANGE:
  {
    // TODO: Add later maybe?
    // _glfwUpdateKeyNamesWin32();
    // break;
  }

  case WM_KEYDOWN:
  case WM_SYSKEYDOWN:
  case WM_KEYUP:
  case WM_SYSKEYUP:
  {
    int key, scancode;
    WallpaperAPI::Input::Action action = (HIWORD(lParam) & KF_UP) ? WallpaperAPI::Input::Action::Release : WallpaperAPI::Input::Action::Press;

    scancode = (HIWORD(lParam) & (KF_EXTENDED | 0xff));
    if (!scancode)
    {
      // NOTE: Some synthetic key messages have a scancode of zero
      // HACK: Map the virtual key back to a usable scancode
      scancode = MapVirtualKeyW((UINT)wParam, MAPVK_VK_TO_VSC);
    }

    key = inputManager->m_keycodes[scancode];

    // The Ctrl keys require special handling
    if (wParam == VK_CONTROL)
    {
      if (HIWORD(lParam) & KF_EXTENDED)
      {
        // Right side keys have the extended key bit set
        key = WallpaperAPI::Input::Key::RightControl;
      }
      else
      {
        // NOTE: Alt Gr sends Left Ctrl followed by Right Alt
        // HACK: We only want one event for Alt Gr, so if we detect
        //       this sequence we discard this Left Ctrl message now
        //       and later report Right Alt normally
        MSG next;
        const DWORD time = GetMessageTime();

        if (PeekMessageW(&next, NULL, 0, 0, PM_NOREMOVE))
        {
          if (next.message == WM_KEYDOWN ||
            next.message == WM_SYSKEYDOWN ||
            next.message == WM_KEYUP ||
            next.message == WM_SYSKEYUP)
          {
            if (next.wParam == VK_MENU &&
              (HIWORD(next.lParam) & KF_EXTENDED) &&
              next.time == time)
            {
              // Next message is Right Alt down so discard this
              break;
            }
          }
        }

        // This is a regular Left Ctrl message
        key = WallpaperAPI::Input::Key::LeftControl;
      }
    }
    else if (wParam == VK_PROCESSKEY)
    {
      // IME notifies that keys have been filtered by setting the
      // virtual key-code to VK_PROCESSKEY
      break;
    }

    if (action == WallpaperAPI::Input::Action::Release && wParam == VK_SHIFT)
    {
      // HACK: Release both Shift keys on Shift up event, as when both
      //       are pressed the first release does not emit any event
      // NOTE: The other half of this is in _glfwPollEventsWin32

      inputManager->HandleKeyAction(WallpaperAPI::Input::Key::LeftShift, scancode, action);
      inputManager->HandleKeyAction(WallpaperAPI::Input::Key::RightShift, scancode, action);
    }
    else if (wParam == VK_SNAPSHOT)
    {
      inputManager->HandleKeyAction(key, scancode, WallpaperAPI::Input::Action::Press);
      inputManager->HandleKeyAction(key, scancode, WallpaperAPI::Input::Action::Release);
    }
    else
    {
      inputManager->HandleKeyAction(key, scancode, action);
    }

    break;
  }

  case WM_LBUTTONDOWN:
  case WM_RBUTTONDOWN:
  case WM_MBUTTONDOWN:
  case WM_XBUTTONDOWN:
  case WM_LBUTTONUP:
  case WM_RBUTTONUP:
  case WM_MBUTTONUP:
  case WM_XBUTTONUP:
  {
    using namespace WallpaperAPI::Input;
    
    int i, button;
    Action action;

    if (uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONUP)
      button = MouseButton::ButtonLeft;
    else if (uMsg == WM_RBUTTONDOWN || uMsg == WM_RBUTTONUP)
      button = MouseButton::ButtonRight;
    else if (uMsg == WM_MBUTTONDOWN || uMsg == WM_MBUTTONUP)
      button = MouseButton::ButtonMiddle;
    else if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1)
      button = MouseButton::Button3;
    else
      button = MouseButton::Button4;

    if (uMsg == WM_LBUTTONDOWN || uMsg == WM_RBUTTONDOWN ||
      uMsg == WM_MBUTTONDOWN || uMsg == WM_XBUTTONDOWN)
    {
      action = Action::Press;
    }
    else
      action = Action::Release;

    for (i = 0; i <= MouseButton::ButtonLast; i++)
    {
      if (inputManager->m_mouseButtons[i] == Action::Press)
        break;
    }

    if (i > MouseButton::ButtonLast)
      SetCapture(hWnd);

    inputManager->HandleMouseAction(button, action);

    for (i = 0; i <= MouseButton::ButtonLast; i++)
    {
      if (inputManager->m_mouseButtons[i] == Action::Press)
        break;
    }

    if (i > MouseButton::ButtonLast)
      ReleaseCapture();

    if (uMsg == WM_XBUTTONDOWN || uMsg == WM_XBUTTONUP)
      return TRUE;

    return 0;
  }

  case WM_MOUSEMOVE:
  {
    const int x = GET_X_LPARAM(lParam);
    const int y = GET_Y_LPARAM(lParam);

    inputManager->HandleCursorPosition(x, y);

    return 0;
  }

  // TODO: Add when you add callback support
  // case WM_MOUSEWHEEL:
  // {
  //   _glfwInputScroll(window, 0.0, (SHORT)HIWORD(wParam) / (double)WHEEL_DELTA);
  //   return 0;
  // }

  // TODO: Add when you add callback support
  // case WM_MOUSEHWHEEL:
  // {
  //   This message is only sent on Windows Vista and later
  //   NOTE: The X-axis is inverted for consistency with macOS and X11
  //   _glfwInputScroll(window, -((SHORT)HIWORD(wParam) / (double)WHEEL_DELTA), 0.0);
  //   return 0;
  // }
  }

  return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}


int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR pCmdLine, _In_ int nCmdShow) {
  const wchar_t CLASS_NAME[] = L"Sample Window Class";
  WNDCLASS wc = { };

  wc.lpfnWndProc = WindowProc;
  wc.hInstance = hInstance;
  wc.lpszClassName = CLASS_NAME;

  if (!RegisterClass(&wc))
  {
    throw std::runtime_error("Couldn't register class!");
  }

  WallpaperAPI::Application app;
  app.Run();

  return 0;
}
