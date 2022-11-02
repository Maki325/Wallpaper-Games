#pragma once
#include "Event.h"
#include "Monitor.h"
#include "../Utils/Utils.h"
#include "../Graphics/Renderer.h"
#include "../Entity/Entity.h"
#include "../Entity/Line.h"
#include "MonitorManager.h"
#include "InputManager.h"

namespace WallpaperAPI
{
  enum AppState
  {
    INITIALIZED,
    RUNNING,
    FAILED
  };

  class Application
  {
  public:
    Application();
    ~Application();

    void Run();
    void Update(float delta);
    void Render();
  private:
    void ResetWallpaper();
  private:
    bool m_running = true;

    HWND m_desktopHWnd;
    HDC m_desktopDC;
    Renderer m_renderer;
    MonitorManager m_monitorManager;
    InputManager m_inputManager;
    AppState m_appState = AppState::INITIALIZED;

    Line m_line;
    float m_rot = 0;

    std::vector<Entity> m_entities;
  };
}
