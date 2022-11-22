#include "pch.h"
#include "Application.h"
#include "Entity/Entity.h"
#include "Layers/ImGuiGLFWLayer.h"
#include "Layers/ImGuiDesktopLayer.h"
#include "imgui.h"
#include "SystemTray/Components/Button.h"

namespace WallpaperAPI
{

  Application::Application()
    : m_desktopHWnd(Utils::GetWallpaperWindow()),
    m_desktopDC(GetDC(m_desktopHWnd)),
    m_renderer(Renderer(m_desktopHWnd, m_desktopDC)),
    m_monitorManager(MonitorManager(m_desktopHWnd, m_desktopDC)),
    m_inputManager(m_desktopHWnd),
    m_imGuiLayer(new ImGuiDesktopLayer(m_desktopHWnd)),
    m_systemTray(SystemTray::SystemTray("Flappy Bird", "resources/icons/flappy-bird.ico"))
  {
    s_app = this;
    srand((unsigned int) time(nullptr));

    GetSystemTray().AddComponent(
      SystemTray::Button(
        "Quit",
        [&] { m_running = false; }
      )
    );

    AddLayer(new GameLayer());
    AddLayer(m_imGuiLayer);
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

    while (m_running)
    {
      std::chrono::milliseconds current = Utils::GetMillis();
      std::chrono::milliseconds elapsed = current - previous;

      float delta = elapsed.count() / 1000.0f;
      for (auto layer : m_layers)
      {
        layer->OnUpdate(delta);
      }

      if (Application::GetApp().GetInputManager().IsKeyDown(Input::Key::Delete))
      {
        m_running = false;
      }

#ifndef WG_DIST
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
#endif

      m_renderer.SwapBuffers();

      frames++;
      if (current - lastFrameTime > SECOND)
      {
        lastFPS = frames;
        frames = 0;
        lastFrameTime = current;
      }

      // previous = Utils::GetMillis() - current;
      m_systemTray.Update();
      // previous = Utils::GetMillis() - previous;
      previous = current;
    }

    for (auto layer : m_layers)
    {
      layer->OnDetach();
    }

    ResetWallpaper();
  }

  void Application::AddLayer(Layer* layer)
  {
    layer->OnAttach();
    m_layers.push_back(layer);
  }

  void Application::RemoveLayer(Layer* layer)
  {
    for (auto it = m_layers.begin();it != m_layers.end();it++)
    {
      auto currentLayer = *it;
      if (currentLayer == layer) {
        layer->OnDetach();
        m_layers.erase(it);
        return;
      }
    }
  }

  void Application::Exit()
  {
    m_running = false;
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
