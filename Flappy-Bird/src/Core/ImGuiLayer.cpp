#include "pch.h"
#include "ImGuiLayer.h"
#include "Application.h"
#include <imgui.h>
#include "ImGuiBackend.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_opengl3.h"

namespace WallpaperAPI
{
  ImGuiLayer::ImGuiLayer(HWND hwnd)
    : Layer("ImGuiLayer"), m_hwnd(hwnd)
  {
  }

  void ImGuiLayer::OnAttach()
  {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    ImGui_ImplWin32_Init(m_hwnd);
    ImGui_ImplOpenGL3_Init("#version 410");
  }

  void ImGuiLayer::OnDetach()
  {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
    ImGui_ImplWin32_Shutdown();
  }

  bool ImGuiLayer::ShouldDetach()
  {
    return false;
  }

  void ImGuiLayer::Begin()
  {
    ImGuiBackend::Update();
    GL_CHECK(glDisable(GL_DEPTH_TEST));
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplWin32_NewFrame();

    ImGuiIO& io = ImGui::GetIO();
    POINT p = { 0, 0 };
    GetCursorPos(&p);
    io.MousePos = ImVec2((float)p.x, (float)p.y);

    ImGui::NewFrame();
    WallpaperAPI::ImGuiBackend::Update();
  }

  void ImGuiLayer::End()
  {
    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    GL_CHECK(glEnable(GL_DEPTH_TEST));
  }

  void ImGuiLayer::OnUpdate(float delta)
  {
  }
  void ImGuiLayer::OnImGuiRender()
  {
  }

}
