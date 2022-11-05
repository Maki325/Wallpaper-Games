#include "pch.h"
#include "ImGuiBackend.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "Application.h"

#ifndef IMGUI_IMPL_WIN32_DISABLE_GAMEPAD
#include <xinput.h>
typedef DWORD(WINAPI* PFN_XInputGetCapabilities)(DWORD, DWORD, XINPUT_CAPABILITIES*);
typedef DWORD(WINAPI* PFN_XInputGetState)(DWORD, XINPUT_STATE*);
#endif

struct ImGui_ImplWin32_Data
{
  HWND                        hWnd;
  HWND                        MouseHwnd;
  bool                        MouseTracked;
  INT64                       Time;
  INT64                       TicksPerSecond;
  ImGuiMouseCursor            LastMouseCursor;
  bool                        HasGamepad;
  bool                        WantUpdateHasGamepad;

#ifndef IMGUI_IMPL_WIN32_DISABLE_GAMEPAD
  HMODULE                     XInputDLL;
  PFN_XInputGetCapabilities   XInputGetCapabilities;
  PFN_XInputGetState          XInputGetState;
#endif

  ImGui_ImplWin32_Data() { memset(this, 0, sizeof(*this)); }
};

static ImGui_ImplWin32_Data* ImGui_ImplWin32_GetBackendData()
{
  return ImGui::GetCurrentContext() ? (ImGui_ImplWin32_Data*)ImGui::GetIO().BackendPlatformUserData : NULL;
}

extern ImGuiContext* GImGui;

namespace WallpaperAPI
{
  namespace ImGuiBackend
  {
    bool Init()
    {
      /*ImGuiIO& io = ImGui::GetIO();
      IM_ASSERT(io.BackendPlatformUserData == NULL && "Already initialized a platform backend!");

      INT64 perf_frequency, perf_counter;
      if (!::QueryPerformanceFrequency((LARGE_INTEGER*)&perf_frequency))
        return false;
      if (!::QueryPerformanceCounter((LARGE_INTEGER*)&perf_counter))
        return false;

      // Setup backend capabilities flags
      ImGui_ImplWin32_Data* bd = IM_NEW(ImGui_ImplWin32_Data)();
      io.BackendPlatformUserData = (void*)bd;
      io.BackendPlatformName = "imgui_impl_win32";
      io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)
      io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;          // We can honor io.WantSetMousePos requests (optional, rarely used)

      bd->hWnd = (HWND)hwnd;
      bd->WantUpdateHasGamepad = true;
      bd->TicksPerSecond = perf_frequency;
      bd->Time = perf_counter;
      bd->LastMouseCursor = ImGuiMouseCursor_COUNT;

      io.ImeWindowHandle = hwnd;

      // Keyboard mapping. Dear ImGui will use those indices to peek into the io.KeysDown[] array that we will update during the application lifetime.
      io.KeyMap[ImGuiKey_Tab] = VK_TAB;
      io.KeyMap[ImGuiKey_LeftArrow] = VK_LEFT;
      io.KeyMap[ImGuiKey_RightArrow] = VK_RIGHT;
      io.KeyMap[ImGuiKey_UpArrow] = VK_UP;
      io.KeyMap[ImGuiKey_DownArrow] = VK_DOWN;
      io.KeyMap[ImGuiKey_PageUp] = VK_PRIOR;
      io.KeyMap[ImGuiKey_PageDown] = VK_NEXT;
      io.KeyMap[ImGuiKey_Home] = VK_HOME;
      io.KeyMap[ImGuiKey_End] = VK_END;
      io.KeyMap[ImGuiKey_Insert] = VK_INSERT;
      io.KeyMap[ImGuiKey_Delete] = VK_DELETE;
      io.KeyMap[ImGuiKey_Backspace] = VK_BACK;
      io.KeyMap[ImGuiKey_Space] = VK_SPACE;
      io.KeyMap[ImGuiKey_Enter] = VK_RETURN;
      io.KeyMap[ImGuiKey_Escape] = VK_ESCAPE;
      io.KeyMap[ImGuiKey_KeyPadEnter] = VK_RETURN;
      io.KeyMap[ImGuiKey_A] = 'A';
      io.KeyMap[ImGuiKey_C] = 'C';
      io.KeyMap[ImGuiKey_V] = 'V';
      io.KeyMap[ImGuiKey_X] = 'X';
      io.KeyMap[ImGuiKey_Y] = 'Y';
      io.KeyMap[ImGuiKey_Z] = 'Z';

      // Dynamically load XInput library
#ifndef IMGUI_IMPL_WIN32_DISABLE_GAMEPAD
      const char* xinput_dll_names[] =
      {
          "xinput1_4.dll",   // Windows 8+
          "xinput1_3.dll",   // DirectX SDK
          "xinput9_1_0.dll", // Windows Vista, Windows 7
          "xinput1_2.dll",   // DirectX SDK
          "xinput1_1.dll"    // DirectX SDK
      };
      for (int n = 0; n < IM_ARRAYSIZE(xinput_dll_names); n++)
        if (HMODULE dll = ::LoadLibraryA(xinput_dll_names[n]))
        {
          bd->XInputDLL = dll;
          bd->XInputGetCapabilities = (PFN_XInputGetCapabilities)::GetProcAddress(dll, "XInputGetCapabilities");
          bd->XInputGetState = (PFN_XInputGetState)::GetProcAddress(dll, "XInputGetState");
          break;
        }
#endif // IMGUI_IMPL_WIN32_DISABLE_GAMEPAD

      return true;*/
      return false;
    }
  
    void Update()
    {
      if (ImGui::GetCurrentContext() == NULL)
        return;

      ImGuiIO& io = ImGui::GetIO();
      ImGui_ImplWin32_Data* bd = ImGui_ImplWin32_GetBackendData();

      InputManager& im = Application::GetApp().GetInputManager();

      for (size_t i = Input::MouseButton::Button0; i < Input::MouseButton::Button4; i++)
      {
        io.MouseDown[i] = im.IsMouseButtonDown((Input::MouseButton) i);
      }
      ImGuiContext& g = *GImGui;
      SetCapture
      // g.HoveredWindow = 0;
      
      /*for (size_t i = Input::Key::Space; i < Input::Key::Last; i++)
      {
        if (i == VK_CONTROL)
        {
          io.KeysDown[VK_LCONTROL] = ((::GetKeyState(VK_LCONTROL) & 0x8000) != 0);
          io.KeysDown[VK_RCONTROL] = ((::GetKeyState(VK_RCONTROL) & 0x8000) != 0);
          io.KeyCtrl = io.KeysDown[VK_LCONTROL] || io.KeysDown[VK_RCONTROL];
          continue;
        }
        if (i == VK_SHIFT)
        {
          io.KeysDown[VK_LSHIFT] = ((::GetKeyState(VK_LSHIFT) & 0x8000) != 0);
          io.KeysDown[VK_RSHIFT] = ((::GetKeyState(VK_RSHIFT) & 0x8000) != 0);
          io.KeyShift = io.KeysDown[VK_LSHIFT] || io.KeysDown[VK_RSHIFT];
          continue;
        }
        if (i == VK_MENU)
        {
          io.KeysDown[VK_LMENU] = ((::GetKeyState(VK_LMENU) & 0x8000) != 0);
          io.KeysDown[VK_RMENU] = ((::GetKeyState(VK_RMENU) & 0x8000) != 0);
          io.KeyAlt = io.KeysDown[VK_LMENU] || io.KeysDown[VK_RMENU];
          continue;
        }

        io.KeysDown[i] = im.IsKeyDown((Input::Key) i);
        if (io.KeysDown[i])
        {
          io.AddInputCharacterUTF16((unsigned short) i);
        }
      }*/

      /*switch (msg)
      {
      case WM_MOUSEMOVE:
        // We need to call TrackMouseEvent in order to receive WM_MOUSELEAVE events
        bd->MouseHwnd = hwnd;
        if (!bd->MouseTracked)
        {
          TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, hwnd, 0 };
          ::TrackMouseEvent(&tme);
          bd->MouseTracked = true;
        }
        break;
      case WM_MOUSELEAVE:
        if (bd->MouseHwnd == hwnd)
          bd->MouseHwnd = NULL;
        bd->MouseTracked = false;
        break;
      case WM_MOUSEWHEEL:
        io.MouseWheel += (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
        return 0;
      case WM_MOUSEHWHEEL:
        io.MouseWheelH += (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
        return 0;
      case WM_SETFOCUS:
      case WM_KILLFOCUS:
        io.AddFocusEvent(msg == WM_SETFOCUS);
        return 0;
      case WM_CHAR:
        // You can also use ToAscii()+GetKeyboardState() to retrieve characters.
        if (wParam > 0 && wParam < 0x10000)
          io.AddInputCharacterUTF16((unsigned short)wParam);
        return 0;
      case WM_SETCURSOR:
        if (LOWORD(lParam) == HTCLIENT && ImGui_ImplWin32_UpdateMouseCursor())
          return 1;
        return 0;
      case WM_DEVICECHANGE:
        if ((UINT)wParam == DBT_DEVNODES_CHANGED)
          bd->WantUpdateHasGamepad = true;
        return 0;
      }
      return 0;*/
    }
  
  }
}
