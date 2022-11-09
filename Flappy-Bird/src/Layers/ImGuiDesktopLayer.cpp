#include <pch.h>
#include "ImGuiDesktopLayer.h"
#include <imgui.h>
#include <backends/imgui_impl_win32.h>
#include <backends/imgui_impl_opengl3.h>
#include "ImGuiBackend.h"

namespace WallpaperAPI
{
  ImGuiDesktopLayer::ImGuiDesktopLayer(HWND hwnd)
    : ImGuiLayer("ImGuiDesktopLayer"), m_hwnd(hwnd)
  {
  }

  void ImGuiDesktopLayer::OnAttach()
  {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(m_hwnd);
    ImGui_ImplOpenGL3_Init("#version 410");
  }

  void ImGuiDesktopLayer::OnDetach()
  {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
  }

  void ImGuiDesktopLayer::Begin()
  {
    GL_CHECK(glDisable(GL_DEPTH_TEST));

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplWin32_NewFrame();

    ImGuiIO& io = ImGui::GetIO();
    POINT p = { 0, 0 };
    GetCursorPos(&p);
    io.MousePos = ImVec2((float)p.x, (float)p.y);

    ImGuiBackend::Update();

    ImGui::NewFrame();
  }

  void ImGuiDesktopLayer::End()
  {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    GL_CHECK(glEnable(GL_DEPTH_TEST));
  }

}
