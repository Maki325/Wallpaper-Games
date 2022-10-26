#pragma once
#include "Event.h"
#include "Monitor.h"
#include "../Utils/ConsoleTask.h"
#include "../Utils/Utils.h"
#include "../Graphics/Renderer.h"
#include "../Entity/Entity.h"
#include "MonitorManager.h"

namespace WallpaperAPI
{
  class Application {
  public:
    Application();
    ~Application();

    void Run();
    static void PostEvent(Event e);
  private:
    bool InitGraphics();
    void ResetWallpaper();
    void GetMonitors();
  private:
    bool m_running = true;

    HWND m_desktopHWnd;
    HDC m_desktopDC;
    Renderer m_renderer;
    MonitorManager m_monitorManager;

    std::vector<Entity> m_entities;

    static std::mutex s_mutex;
    static std::queue<Event> s_messages;
  };
}
