#include "pch.h"
#include "Application.h"
#include "Entity/Entity.h"
#include "ImGuiLayer.h"
#include "imgui.h"

namespace WallpaperAPI
{

  Application::Application()
    : m_desktopHWnd(Utils::GetWallpaperWindow()),
    m_desktopDC(GetDC(m_desktopHWnd)),
    m_renderer(Renderer(m_desktopHWnd, m_desktopDC)),
    m_monitorManager(MonitorManager(m_desktopHWnd, m_desktopDC)),
    m_inputManager(m_desktopHWnd),
    m_imGuiLayer(m_desktopHWnd)
  {
    srand(time(nullptr));

    m_layers.push_back(new GameLayer());

    s_app = this;
  }

  Application::~Application() {}

  void Application::Run()
  {
    if (!m_renderer.IsInitialized()) {
      std::cout << "Failed to initialize OpenGL context" << std::endl;
      std::cin.get();
      return;
    }
    std::cout << "Successfully initialized OpenGL context" << std::endl;

    if (m_monitorManager.GetMonitors().empty()) {
      std::cout << "No monitor available!" << std::endl;
      std::cin.get();
      return;
    }

    Monitor& monitor = m_monitorManager.GetMonitors().at(0);
    m_renderer.SetViewport(monitor.area);

    std::chrono::milliseconds previous = Utils::GetMillis();
    std::chrono::milliseconds lastFrameTime = previous;
    size_t frames = 0;
    std::chrono::milliseconds lag(0);
    const std::chrono::milliseconds MS_PER_UPDATE(16);
    const std::chrono::milliseconds SECOND(1000);

    m_imGuiLayer.OnAttach();
    for (Layer* layer : m_layers)
    {
      layer->OnAttach();
    }

    while (true)
    {
      std::chrono::milliseconds current = Utils::GetMillis();
      std::chrono::milliseconds elapsed = current - previous;

      for (auto it = m_layers.begin();it != m_layers.end();)
      {
        if ((*it)->ShouldDetach())
        {
          it = m_layers.erase(it);
          continue;
        }
        (*it)->OnUpdate(elapsed.count() / 1000.0);
        it++;
      }

      bool shutdown = false;
      m_imGuiLayer.Begin();
      {
        for (Layer* layer : m_layers)
        {
          layer->OnImGuiRender();
        }
        ImGui::Begin("Flappy Bird");
        ImGui::Text("FPS: %.3f", frames / ((current - lastFrameTime).count() / 1000.0));

        shutdown = ImGui::Button("Shut down!");
        ImGui::End();
      }
      m_imGuiLayer.End();

      m_renderer.SwapBuffers();

      frames++;
      if (current - lastFrameTime > SECOND)
      {
        std::cout << "Frames: " << frames << std::endl;
        frames = 0;
        lastFrameTime = current;
      }

      previous = current;
      if (shutdown)
      {
        std::cout << "Shutdown" << std::endl;
        break;
      }
    }

    m_imGuiLayer.OnDetach();
    for (Layer* layer : m_layers)
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
