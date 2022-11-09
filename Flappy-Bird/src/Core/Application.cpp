#include "pch.h"
#include "Application.h"
#include "Entity/Entity.h"
#include "Layers/ImGuiGLFWLayer.h"
#include "Layers/ImGuiDesktopLayer.h"
#include "imgui.h"

namespace WallpaperAPI
{

  Application::Application()
    : m_desktopHWnd(Utils::GetWallpaperWindow()),
    m_desktopDC(GetDC(m_desktopHWnd)),
    m_renderer(Renderer(m_desktopHWnd, m_desktopDC)),
    m_monitorManager(MonitorManager(m_desktopHWnd, m_desktopDC)),
    m_inputManager(m_desktopHWnd),
    m_imGuiLayer(new ImGuiDesktopLayer(m_desktopHWnd))
  {
    srand((unsigned int) time(nullptr));

    m_layers.push_back(new GameLayer());

    s_app = this;
  }

  Application::~Application() {}

  void Application::Run()
  {
    if (!m_renderer.IsInitialized()) {
      INFO("Failed to initialize OpenGL context");
      std::cin.get();
      return;
    }
    INFO("Successfully initialized OpenGL context");

    if (m_monitorManager.GetMonitors().empty()) {
      INFO("No monitor available!");
      std::cin.get();
      return;
    }

    Monitor& monitor = m_monitorManager.GetMonitors().at(0);
    m_renderer.SetViewport(monitor.area);

    std::chrono::milliseconds previous = Utils::GetMillis();
    std::chrono::milliseconds lastFrameTime = previous;
    size_t frames = 0;
    const std::chrono::milliseconds SECOND(1000);

    m_imGuiLayer->OnAttach();
    for (Layer* layer : m_layers)
    {
      layer->OnAttach();
    }

    while (m_running)
    {
      std::chrono::milliseconds current = Utils::GetMillis();
      std::chrono::milliseconds elapsed = current - previous;

      for (auto layer : m_layers)
      {
        layer->OnUpdate(elapsed.count() / 1000.0f);
      }

      m_imGuiLayer->Begin();
      {
        for (auto layer : m_layers)
        {
          layer->OnImGuiRender();
        }
        ImGui::Begin("Flappy Bird");
        ImGui::Text("FPS: %zu (%.3f)", lastFPS, frames / ((current - lastFrameTime).count() / 1000.0));

        if (ImGui::Button("Shut down!")) { m_running = false; }
        ImGui::End();
      }
      m_imGuiLayer->End();

      m_renderer.SwapBuffers();

      frames++;
      if (current - lastFrameTime > SECOND)
      {
        lastFPS = frames;
        frames = 0;
        lastFrameTime = current;
      }

      previous = current;
    }

    m_imGuiLayer->OnDetach();
    for (auto layer : m_layers)
    {
      layer->OnDetach();
    }

    ResetWallpaper();
  }

  void Application::ResetWallpaper() {
    WCHAR path[MAX_PATH];
    SystemParametersInfo(SPI_GETDESKWALLPAPER, MAX_PATH, &path[0], 0);
    SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, &path[0], SPIF_UPDATEINIFILE);

    std::wcout << "Path(" << wcsnlen(path, 128) << "): ";
    for (int i = 0; i < wcsnlen(path, 128); i++) {
      std::wcout << path[i];
    }
    std::wcout << std::endl;
  }

  Application* Application::s_app = nullptr;
}
