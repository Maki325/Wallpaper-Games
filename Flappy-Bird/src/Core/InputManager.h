#pragma once

namespace WallpaperAPI
{
  namespace Input {
    enum Key // Done using ([A-Z])([A-Z]+ ) - $1\L$2
    {
      Sticky = -2,
      /* The unknown key */
      Unknown = -1,

      /* Printable keys */
      Space = 32,
      Apostrophe = 39,  /* ' */
      Comma = 44,  /* , */
      Minus = 45,  /* - */
      Period = 46,  /* . */
      Slash = 47,  /* / */

      D0 = 48,
      D1 = 49,
      D2 = 50,
      D3 = 51,
      D4 = 52,
      D5 = 53,
      D6 = 54,
      D7 = 55,
      D8 = 56,
      D9 = 57,

      Semicolon = 59,  /* ; */
      Equal = 61,  /* = */

      A = 65,
      B = 66,
      C = 67,
      D = 68,
      E = 69,
      F = 70,
      G = 71,
      H = 72,
      I = 73,
      J = 74,
      K = 75,
      L = 76,
      M = 77,
      N = 78,
      O = 79,
      P = 80,
      Q = 81,
      R = 82,
      S = 83,
      T = 84,
      U = 85,
      V = 86,
      W = 87,
      X = 88,
      Y = 89,
      Z = 90,

      LeftBracket = 91,  /* [ */
      Backslash = 92,  /* \ */
      RightBracket = 93,  /* ] */
      GraveAccent = 96,  /* ` */

      World1 = 161, /* non-US #1 */
      World2 = 162, /* non-US #2 */

      /* Function keys */
      Escape = VK_ESCAPE,
      Enter = VK_RETURN,
      Tab = VK_TAB,
      Backspace = VK_BACK,
      Insert = VK_INSERT,
      Delete = VK_DELETE,
      Right = VK_RIGHT,
      Left = VK_LEFT,
      Down = VK_DOWN,
      Up = VK_UP,
      PageUp = VK_PRIOR,
      PageDown = VK_NEXT,
      Home = VK_HOME,
      End = VK_END,
      CapsLock = VK_CAPITAL,
      ScrollLock = VK_SCROLL,
      NumLock = VK_NUMLOCK,
      PrintScreen = VK_SNAPSHOT,
      Pause = VK_PAUSE,
      F1 = VK_F1,
      F2 = VK_F2,
      F3 = VK_F3,
      F4 = VK_F4,
      F5 = VK_F5,
      F6 = VK_F6,
      F7 = VK_F7,
      F8 = VK_F8,
      F9 = VK_F9,
      F10 = VK_F10,
      F11 = VK_F11,
      F12 = VK_F12,
      F13 = VK_F13,
      F14 = VK_F14,
      F15 = VK_F15,
      F16 = VK_F16,
      F17 = VK_F17,
      F18 = VK_F18,
      F19 = VK_F19,
      F20 = VK_F20,
      F21 = VK_F21,
      F22 = VK_F22,
      F23 = VK_F23,
      F24 = VK_F24,
      F25 = 314,

      /* Keypad */
      KP0 = VK_NUMPAD0,
      KP1 = VK_NUMPAD1,
      KP2 = VK_NUMPAD2,
      KP3 = VK_NUMPAD3,
      KP4 = VK_NUMPAD4,
      KP5 = VK_NUMPAD5,
      KP6 = VK_NUMPAD6,
      KP7 = VK_NUMPAD7,
      KP8 = VK_NUMPAD8,
      KP9 = VK_NUMPAD9,
      KPDecimal = VK_DECIMAL,
      KPDivide = VK_DIVIDE,
      KPMultiply = VK_MULTIPLY,
      KPSubtract = VK_SUBTRACT,
      KPAdd = VK_ADD,
      KPEnter = VK_RETURN,
      KPEqual = 336,

      LeftShift = VK_LSHIFT,
      LeftControl = VK_LCONTROL,
      LeftAlt = VK_LMENU,
      LeftSuper = VK_LWIN,
      RightShift = VK_RSHIFT,
      RightControl = VK_RCONTROL,
      RightAlt = VK_RMENU,
      RightSuper = VK_RWIN,
      Menu = VK_MENU,

      Last = Menu,
    };

    enum MouseButton
    {
      Button0 = 0,
      Button1,
      Button2,
      Button3,
      Button4,
      Button5,
      Button6,
      Button7,

      ButtonLast = Button7,
      ButtonLeft = Button0,
      ButtonRight = Button1,
      ButtonMiddle = Button2
    };

    enum Action
    {
      Release = 0,
      Press,
      Repeat
    };
  }

  class InputManager
  {
  public:
    InputManager(HWND hWnd);

    // Pass the key in upper case
    bool IsKeyDown(Input::Key key);
    bool IsMouseButtonDown(Input::MouseButton button);
    glm::ivec2 GetMousePosition();

    void InitKeys();
    void InitMouse();

    void HandleKeyAction(int key, int scancode, Input::Action action);
    void HandleMouseAction(int button, Input::Action action);
    void HandleCursorPosition(double xpos, double ypos);

    short int m_keycodes[512];
    short int m_scancodes[Input::Key::Last + 1];
    char m_keynames[Input::Key::Last + 1][5];

    Input::Action m_keys[Input::Key::Last + 1];
    Input::Action m_mouseButtons[Input::MouseButton::ButtonLast + 1];
    glm::vec2 m_mousePosition;
  private:
    HWND m_hWnd;
  };
}

