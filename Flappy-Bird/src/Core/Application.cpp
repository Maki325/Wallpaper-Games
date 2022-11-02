#include "pch.h"
#include "Application.h"
#include "Entity/Entity.h"

namespace WallpaperAPI
{

  Application::Application()
    : m_desktopHWnd(Utils::GetWallpaperWindow()),
        m_desktopDC(GetDC(m_desktopHWnd)), m_renderer(Renderer(m_desktopHWnd, m_desktopDC)),
        m_monitorManager(MonitorManager(m_desktopHWnd, m_desktopDC)), m_inputManager(m_desktopHWnd),
        m_line(glm::vec3(-1.0f, 0, 0.0), glm::vec3(1.0f, 0, 0.0), glm::vec4(0, 0, 1, 1)),
        m_player({
          {
            // positions              // texture coords
            { { 1.0f,  1.0f, 0.0f},   {1.0f, 1.0f} }, // top right
            { { 1.0f, -1.0f, 0.0f},   {1.0f, 0.0f} }, // bottom right
            { {-1.0f, -1.0f, 0.0f},   {0.0f, 0.0f} }, // bottom left
            { {-1.0f,  1.0f, 0.0f},   {0.0f, 1.0f} }, // top left
          }, {  // note that we start from 0!
            { 0, 1, 3 }, // first Triangle
            { 1, 2, 3 }  // second Triangle
          }
        }, glm::vec3(-1, 0, 0), glm::vec3(0), glm::vec3(0.25), glm::vec3(0), "resources/textures/flappy-bird.png")
  {
    srand(time(nullptr));

    Model model{
      {
        // positions              // texture coords
        { { 1.0f,  1.0f, 0.0f},   {1.0f, 1.0f} }, // top right
        { { 1.0f, -1.0f, 0.0f},   {1.0f, 0.0f} }, // bottom right
        { {-1.0f, -1.0f, 0.0f},   {0.0f, 0.0f} }, // bottom left
        { {-1.0f,  1.0f, 0.0f},   {0.0f, 1.0f} }, // top left
      },
      {  // note that we start from 0!
        { 0, 1, 3 }, // first Triangle
        { 1, 2, 3 }  // second Triangle
      }
    };

    m_ground.emplace_back(model, glm::vec3(-1.22, -0.2, 0), glm::vec3(0), glm::vec3(1), glm::vec3(0), "resources/textures/Ground.png");
    m_ground.emplace_back(model, glm::vec3( 0.50, -0.2, 0), glm::vec3(0), glm::vec3(1), glm::vec3(0), "resources/textures/Ground.png");
    m_ground.emplace_back(model, glm::vec3( 2.22, -0.2, 0), glm::vec3(0), glm::vec3(1), glm::vec3(0), "resources/textures/Ground.png");
    m_ground.emplace_back(model, glm::vec3( 3.94, -0.2, 0), glm::vec3(0), glm::vec3(1), glm::vec3(0), "resources/textures/Ground.png");

    m_aabbs.emplace_back(glm::vec2(-1,  0.0), glm::vec2(0.25));
    m_aabbs.emplace_back(glm::vec2( 0, -1.0), glm::vec2(10, 0.5));
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
    std::chrono::milliseconds start = previous;
    std::chrono::milliseconds lag(0);
    const std::chrono::milliseconds MS_PER_UPDATE(16);
    const std::chrono::milliseconds sec(10 * 1000);

    while (Utils::GetMillis() < start + sec)
    {
      std::chrono::milliseconds current = Utils::GetMillis();
      std::chrono::milliseconds elapsed = current - previous;

      Update(elapsed.count() / 1000.0);

      Render();
      previous = current;
    }

    std::cout << "\nPress any key to exit...";
    std::cin.get();
    ResetWallpaper();
  }

  void Application::Update(float delta)
  {
    switch (m_appState)
    {
    case WallpaperAPI::INITIALIZED:
      UpdateInitialized(delta);
      break;
    case WallpaperAPI::RUNNING:
      UpdateRunning(delta);
      break;
    case WallpaperAPI::FAILED:
      break;
    default:
      break;
    }
  }

  void Application::UpdateInitialized(float delta)
  {
    ScrollGround(delta);
    if (m_inputManager.IsKeyDown(Input::Key::Space))
    {
      m_appState = AppState::RUNNING;
      m_player.m_velocity.y = 6.0f;
    }
  }

  void Application::UpdateRunning(float delta)
  {
    ScrollGround(delta);
    glm::vec3 GRAVITY(0, -4.0f, 0);

    glm::vec3 velocity(GRAVITY);

    velocity += m_player.m_velocity;
    if (m_player.m_velocity.y > 0) {
      m_player.m_velocity += GRAVITY * delta;
    }
    else {
      m_player.m_velocity.y = 0;
    }

    m_player.m_position += (velocity * delta);

    if (m_inputManager.IsKeyDown(Input::Key::Space))
    {
      m_player.m_velocity.y = 6.0f;
    }

    AABB& playerAABB = m_aabbs.at(0);
    playerAABB.m_position.y = m_player.m_position.y;

    if (AABB::IsColliding(playerAABB, m_aabbs.at(1)))
    {
      m_appState = AppState::FAILED;
    }
  }

  void Application::UpdateFailed(float delta)
  {}

  void Application::ScrollGround(float delta)
  {
    for (Entity& ground : m_ground)
    {
      ground.m_position.x -= 1.0 * delta;

      if (ground.m_position.x <= -1.72 * 1.75) {
        ground.m_position.x += 1.72 * 4;
      }
    }
  }

  void Application::Render()
  {
    ShaderProgram& shader = m_renderer.GetShaderProgram();

    GL_CHECK(glClearColor(0.2f, 0.3f, 0.9f, 1.0f));
    GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    shader.Use();
    m_renderer.RenderEntity(m_player);
    for (Entity& entity : m_ground)
    {
      m_renderer.RenderEntity(entity);
    }

    // m_renderer.RenderLine(m_line, glm::vec3(-1.22, -0.2, 0)); // It's center based?
    // m_renderer.RenderLine(m_line, glm::vec3(0));

    // m_renderer.RenderLine(m_line, glm::vec3(0, -1.0, 0));

    m_renderer.SwapBuffers();
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
}
