#pragma once
#include "Event.h"
#include "Monitor.h"
#include "Utils/Utils.h"
#include "Graphics/Renderer.h"
#include "Entity/Entity.h"
#include "Layer.h"
#include "Game/GameLayer.h"
#include "Colliders/AABB.h"
#include "MonitorManager.h"
#include "InputManager.h"
#include "Layers/ImGuiLayer.h"
#include "SystemTray/SystemTray.h"

namespace WallpaperAPI
{
  class Application
  {
  public:
    Application();
    ~Application();

    void Run();

    void AddLayer(Layer* layer);
    void RemoveLayer(Layer* layer);

    void Exit();

    static Application& GetApp() { return *s_app; }

    Renderer& GetRenderer() { return m_renderer; }
    MonitorManager& GetMonitorManager() { return m_monitorManager; }
    InputManager& GetInputManager() { return m_inputManager; }
    SystemTray::SystemTray& GetSystemTray() { return m_systemTray; }
  private:
    void ResetWallpaper();
  private:
    bool m_running = true;
    size_t lastFPS = 0;

    HWND m_desktopHWnd;
    HDC m_desktopDC;
    Renderer m_renderer;
    MonitorManager m_monitorManager;
    InputManager m_inputManager;

    SystemTray::SystemTray m_systemTray;

    std::vector<Layer*> m_layers;
    ImGuiLayer *m_imGuiLayer;
    static Application* s_app;
  };
}
