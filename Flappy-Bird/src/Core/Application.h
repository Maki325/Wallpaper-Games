#pragma once
#include "Event.h"
#include "Monitor.h"
#include "../Utils/Utils.h"
#include "../Graphics/Renderer.h"
#include "../Entity/Entity.h"
#include "../Entity/Line.h"
#include "AABB.h"
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
    void UpdateInitialized(float delta);
    void UpdateRunning(float delta);
    void UpdateFailed(float delta);
    void ScrollGround(float delta);
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

    Entity m_player;
    std::vector<Entity> m_ground;

    std::vector<AABB> m_aabbs;
  };
}
