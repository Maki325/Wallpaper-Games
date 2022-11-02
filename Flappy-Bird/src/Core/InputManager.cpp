#include <pch.h>
#include "InputManager.h"

namespace WallpaperAPI
{
  InputManager::InputManager(HWND hWnd)
    : m_hWnd(hWnd) {
    SetPropW(hWnd, L"InputManager", this);

    InitKeys();
    InitMouse();
  }

  bool InputManager::IsKeyDown(Input::Key key)
  {
    return GetKeyState(key) & 0x8000;
  }

  bool InputManager::IsMouseButtonDown(Input::MouseButton button)
  {
    switch (button)
    {
    case WallpaperAPI::Input::Button0:
      return GetKeyState(VK_LBUTTON) & 0x8000;
    case WallpaperAPI::Input::Button1:
      return GetKeyState(VK_RBUTTON) & 0x8000;
    case WallpaperAPI::Input::Button2:
      return GetKeyState(VK_MBUTTON) & 0x8000;
    case WallpaperAPI::Input::Button3:
      return GetKeyState(VK_XBUTTON1) & 0x8000;
    case WallpaperAPI::Input::Button4:
      return GetKeyState(VK_XBUTTON2) & 0x8000;
    }
    return false;
  }

  glm::ivec2 InputManager::GetMousePosition()
  {
    POINT p = {0, 0};
    GetCursorPos(&p);
    return glm::ivec2(p.x, p.y);
  }

  void InputManager::HandleKeyAction(int key, int scancode, Input::Action action)
  {
    if (key >= 0 && key <= Input::Key::Last)
    {
      if (action == Input::Action::Release && m_keys[key] == Input::Action::Release)
        return;

      m_keys[key] = action;
    }
  }

  void InputManager::HandleMouseAction(int button, Input::Action action)
  {
    if (button < 0 || button > Input::MouseButton::ButtonLast)
      return;

    m_mouseButtons[button] = action;
  }

  void InputManager::HandleCursorPosition(double xpos, double ypos)
  {
    if (m_mousePosition.x == xpos && m_mousePosition.y == ypos)
      return;

    m_mousePosition.x = xpos;
    m_mousePosition.y = ypos;
  }

  void InputManager::InitKeys()
  {
    memset(m_keycodes, -1, sizeof(m_keycodes));
    memset(m_scancodes, -1, sizeof(m_scancodes));

    memset(m_keys, Input::Action::Release, sizeof(m_keys));

    #pragma region KeyCodes
    m_keycodes[0x00B] = Input::Key::D0;
    m_keycodes[0x002] = Input::Key::D1;
    m_keycodes[0x003] = Input::Key::D2;
    m_keycodes[0x004] = Input::Key::D3;
    m_keycodes[0x005] = Input::Key::D4;
    m_keycodes[0x006] = Input::Key::D5;
    m_keycodes[0x007] = Input::Key::D6;
    m_keycodes[0x008] = Input::Key::D7;
    m_keycodes[0x009] = Input::Key::D8;
    m_keycodes[0x00A] = Input::Key::D9;
    m_keycodes[0x01E] = Input::Key::A;
    m_keycodes[0x030] = Input::Key::B;
    m_keycodes[0x02E] = Input::Key::C;
    m_keycodes[0x020] = Input::Key::D;
    m_keycodes[0x012] = Input::Key::E;
    m_keycodes[0x021] = Input::Key::F;
    m_keycodes[0x022] = Input::Key::G;
    m_keycodes[0x023] = Input::Key::H;
    m_keycodes[0x017] = Input::Key::I;
    m_keycodes[0x024] = Input::Key::J;
    m_keycodes[0x025] = Input::Key::K;
    m_keycodes[0x026] = Input::Key::L;
    m_keycodes[0x032] = Input::Key::M;
    m_keycodes[0x031] = Input::Key::N;
    m_keycodes[0x018] = Input::Key::O;
    m_keycodes[0x019] = Input::Key::P;
    m_keycodes[0x010] = Input::Key::Q;
    m_keycodes[0x013] = Input::Key::R;
    m_keycodes[0x01F] = Input::Key::S;
    m_keycodes[0x014] = Input::Key::T;
    m_keycodes[0x016] = Input::Key::U;
    m_keycodes[0x02F] = Input::Key::V;
    m_keycodes[0x011] = Input::Key::W;
    m_keycodes[0x02D] = Input::Key::X;
    m_keycodes[0x015] = Input::Key::Y;
    m_keycodes[0x02C] = Input::Key::Z;

    m_keycodes[0x028] = Input::Key::Apostrophe;
    m_keycodes[0x02B] = Input::Key::Backslash;
    m_keycodes[0x033] = Input::Key::Comma;
    m_keycodes[0x00D] = Input::Key::Equal;
    m_keycodes[0x029] = Input::Key::GraveAccent;
    m_keycodes[0x01A] = Input::Key::LeftBracket;
    m_keycodes[0x00C] = Input::Key::Minus;
    m_keycodes[0x034] = Input::Key::Period;
    m_keycodes[0x01B] = Input::Key::RightBracket;
    m_keycodes[0x027] = Input::Key::Semicolon;
    m_keycodes[0x035] = Input::Key::Slash;
    m_keycodes[0x056] = Input::Key::World2;

    m_keycodes[0x00E] = Input::Key::Backspace;
    m_keycodes[0x153] = Input::Key::Delete;
    m_keycodes[0x14F] = Input::Key::End;
    m_keycodes[0x01C] = Input::Key::Enter;
    m_keycodes[0x001] = Input::Key::Escape;
    m_keycodes[0x147] = Input::Key::Home;
    m_keycodes[0x152] = Input::Key::Insert;
    m_keycodes[0x15D] = Input::Key::Menu;
    m_keycodes[0x151] = Input::Key::PageDown;
    m_keycodes[0x149] = Input::Key::PageUp;
    m_keycodes[0x045] = Input::Key::Pause;
    m_keycodes[0x146] = Input::Key::Pause;
    m_keycodes[0x039] = Input::Key::Space;
    m_keycodes[0x00F] = Input::Key::Tab;
    m_keycodes[0x03A] = Input::Key::CapsLock;
    m_keycodes[0x145] = Input::Key::NumLock;
    m_keycodes[0x046] = Input::Key::ScrollLock;
    m_keycodes[0x03B] = Input::Key::F1;
    m_keycodes[0x03C] = Input::Key::F2;
    m_keycodes[0x03D] = Input::Key::F3;
    m_keycodes[0x03E] = Input::Key::F4;
    m_keycodes[0x03F] = Input::Key::F5;
    m_keycodes[0x040] = Input::Key::F6;
    m_keycodes[0x041] = Input::Key::F7;
    m_keycodes[0x042] = Input::Key::F8;
    m_keycodes[0x043] = Input::Key::F9;
    m_keycodes[0x044] = Input::Key::F10;
    m_keycodes[0x057] = Input::Key::F11;
    m_keycodes[0x058] = Input::Key::F12;
    m_keycodes[0x064] = Input::Key::F13;
    m_keycodes[0x065] = Input::Key::F14;
    m_keycodes[0x066] = Input::Key::F15;
    m_keycodes[0x067] = Input::Key::F16;
    m_keycodes[0x068] = Input::Key::F17;
    m_keycodes[0x069] = Input::Key::F18;
    m_keycodes[0x06A] = Input::Key::F19;
    m_keycodes[0x06B] = Input::Key::F20;
    m_keycodes[0x06C] = Input::Key::F21;
    m_keycodes[0x06D] = Input::Key::F22;
    m_keycodes[0x06E] = Input::Key::F23;
    m_keycodes[0x076] = Input::Key::F24;
    m_keycodes[0x038] = Input::Key::LeftAlt;
    m_keycodes[0x01D] = Input::Key::LeftControl;
    m_keycodes[0x02A] = Input::Key::LeftShift;
    m_keycodes[0x15B] = Input::Key::LeftSuper;
    m_keycodes[0x137] = Input::Key::PrintScreen;
    m_keycodes[0x138] = Input::Key::RightAlt;
    m_keycodes[0x11D] = Input::Key::RightControl;
    m_keycodes[0x036] = Input::Key::RightShift;
    m_keycodes[0x15C] = Input::Key::RightSuper;
    m_keycodes[0x150] = Input::Key::Down;
    m_keycodes[0x14B] = Input::Key::Left;
    m_keycodes[0x14D] = Input::Key::Right;
    m_keycodes[0x148] = Input::Key::Up;

    m_keycodes[0x052] = Input::Key::KP0;
    m_keycodes[0x04F] = Input::Key::KP1;
    m_keycodes[0x050] = Input::Key::KP2;
    m_keycodes[0x051] = Input::Key::KP3;
    m_keycodes[0x04B] = Input::Key::KP4;
    m_keycodes[0x04C] = Input::Key::KP5;
    m_keycodes[0x04D] = Input::Key::KP6;
    m_keycodes[0x047] = Input::Key::KP7;
    m_keycodes[0x048] = Input::Key::KP8;
    m_keycodes[0x049] = Input::Key::KP9;
    m_keycodes[0x04E] = Input::Key::KPAdd;
    m_keycodes[0x053] = Input::Key::KPDecimal;
    m_keycodes[0x135] = Input::Key::KPDivide;
    m_keycodes[0x11C] = Input::Key::KPEnter;
    m_keycodes[0x059] = Input::Key::KPEqual;
    m_keycodes[0x037] = Input::Key::KPMultiply;
    m_keycodes[0x04A] = Input::Key::KPSubtract;
    #pragma endregion KeyCodes

    for (int scancode = 0; scancode < 512; scancode++)
    {
      if (m_keycodes[scancode] > 0 && m_keycodes[scancode] < Input::Key::Last + 1)
        m_scancodes[m_keycodes[scancode]] = scancode;
    }
  }

  void InputManager::InitMouse()
  {
    memset(m_mouseButtons, Input::Action::Release, sizeof(m_mouseButtons));
  }
}
