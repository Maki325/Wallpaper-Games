#pragma once
#include "Event.h"
#include "Monitor.h"
#include "../Utils/ConsoleTask.h"
#include "../Utils/Utils.h"
#include "../Graphics/Renderer.h"
#include "../Entity/Entity.h"
#include "MonitorManager.h"
#include "InputManager.h"

namespace WallpaperAPI
{
  class Application {
  public:
    Application();
    ~Application();

    void Run();
    static void PostEvent(Event e);
    void ProcessInput();
    void Update();
    void Render();
  private:
    void ResetWallpaper();
    void GetMonitors();
  private:
    bool m_running = true;

    HWND m_desktopHWnd;
    HDC m_desktopDC;
    Renderer m_renderer;
    MonitorManager m_monitorManager;
    InputManager m_inputManager;

    std::vector<Entity> m_entities;

    static std::mutex s_mutex;
    static std::queue<Event> s_messages;
  };
}
