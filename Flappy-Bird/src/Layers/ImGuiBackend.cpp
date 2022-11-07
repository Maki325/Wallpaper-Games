#include "pch.h"
#include "ImGuiBackend.h"
#include <imgui.h>
#include "Core/Application.h"

namespace WallpaperAPI
{
  namespace ImGuiBackend
  {
    void Update()
    {
      if (ImGui::GetCurrentContext() == NULL)
        return;

      ImGuiIO& io = ImGui::GetIO();
      InputManager& im = Application::GetApp().GetInputManager();

      for (size_t i = Input::MouseButton::Button0; i < Input::MouseButton::Button4; i++)
      {
        io.MouseDown[i] = im.IsMouseButtonDown((Input::MouseButton) i);
      }

      for (size_t i = 0; i < Input::Key::Last; i++)
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

        bool last = io.KeysDown[i];
        io.KeysDown[i] = im.IsKeyDown((Input::Key) i);

        if (!last && io.KeysDown[i])
        {
          size_t key = i;
          if (key >= Input::Key::KP0 && key <= Input::Key::KP9)
          {
            key = i - Input::Key::KP0 + Input::Key::D0;
          }
          else if (key == Input::Key::KPSubtract)
          {
            key = Input::Key::Minus;
          }
          else if (key == Input::Key::KPDecimal || key == VK_OEM_PERIOD)
          {
            key = Input::Key::Period;
          }

          io.AddInputCharacterUTF16((unsigned short) key);
        }
      }
    }
  
  }
}
