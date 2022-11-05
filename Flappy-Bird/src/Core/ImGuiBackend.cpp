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

static ImGui_ImplWin32_Data* ImGui_ImplWin32_GetBackendData_Custom()
{
  return ImGui::GetCurrentContext() ? (ImGui_ImplWin32_Data*)ImGui::GetIO().BackendPlatformUserData : NULL;
}

namespace WallpaperAPI
{
  namespace ImGuiBackend
  {
    void Update()
    {
      if (ImGui::GetCurrentContext() == NULL)
        return;

      ImGuiIO& io = ImGui::GetIO();
      ImGui_ImplWin32_Data* bd = ImGui_ImplWin32_GetBackendData_Custom();

      InputManager& im = Application::GetApp().GetInputManager();

      for (size_t i = Input::MouseButton::Button0; i < Input::MouseButton::Button4; i++)
      {
        io.MouseDown[i] = im.IsMouseButtonDown((Input::MouseButton) i);
      }

      for (size_t i = Input::Key::Space; i < Input::Key::Last; i++)
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
      }
    }
  
  }
}
