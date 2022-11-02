#include "pch.h"
#include "Application.h"
#include "Entity/Entity.h"

namespace WallpaperAPI
{

  Application::Application()
    : m_desktopHWnd(Utils::GetWallpaperWindow()),
        m_desktopDC(GetDC(m_desktopHWnd)), m_renderer(Renderer(m_desktopHWnd, m_desktopDC)),
        m_monitorManager(MonitorManager(m_desktopHWnd, m_desktopDC)), m_inputManager(m_desktopHWnd),
        // m_line(glm::vec3(0), glm::vec3(0, 1, 0), glm::vec4(0.2, 0.2, 0.7, 1))
        // m_line(glm::vec3(-1.22, -0.2, 0), glm::vec3(0.78, 1.8, 0), glm::vec4(0, 0, 1, 1))
        m_line(glm::vec3(-1.0f, 0, 0.0), glm::vec3(1.0f, 0, 0.0), glm::vec4(0, 0, 1, 1))
  {
    srand(time(nullptr));

    Vertex vertices[] = {
      // positions              // texture coords
      { { 1.0f,  1.0f, 0.0f},   {1.0f, 1.0f} }, // top right
      { { 1.0f, -1.0f, 0.0f},   {1.0f, 0.0f} }, // bottom right
      { {-1.0f, -1.0f, 0.0f},   {0.0f, 0.0f} }, // bottom left
      { {-1.0f,  1.0f, 0.0f},   {0.0f, 1.0f} }, // top left
    };
    glm::uvec3 indices[] = {  // note that we start from 0!
      { 0, 1, 3 }, // first Triangle
      { 1, 2, 3 }  // second Triangle
    };

    std::vector<Vertex> verticesVec(vertices, vertices + 4);
    std::vector<glm::uvec3> indicesVec(indices, indices + 2);
    Model model{ verticesVec , indicesVec };


    Vertex vertices2[] = {
      // positions              // texture coords
      { { 1.0f,  1.0f, 0.0f},   {1.0f, 1.0f} }, // top right
      { { 1.0f, -1.0f, 0.0f},   {1.0f, 0.0f} }, // bottom right
      { {-1.0f, -1.0f, 0.0f},   {0.0f, 0.0f} }, // bottom left
      { {-1.0f,  1.0f, 0.0f},   {0.0f, 1.0f} }, // top left
    };
    glm::uvec3 indices2[] = {  // note that we start from 0!
      { 0, 1, 3 }, // first Triangle
      { 1, 2, 3 }  // second Triangle
    };
    std::vector<Vertex> verticesVec2(vertices2, vertices2 + 4);
    std::vector<glm::uvec3> indicesVec2(indices2, indices2 + 2);
    Model model2{ verticesVec2 , indicesVec2 };
    m_entities.emplace_back(model, glm::vec3(-1, 0, 0), glm::vec3(0), glm::vec3(0.25), glm::vec3(0), "resources/textures/flappy-bird.png");

    m_entities.emplace_back(model2, glm::vec3(-1.22, -0.2, 0), glm::vec3(0), glm::vec3(1), glm::vec3(0), "resources/textures/Ground.png");
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
    m_rot += 3 * 360 * delta;
    if (m_rot >= 360) m_rot -= 360;
    Entity& player = m_entities.at(0);
    switch (m_appState)
    {
    case WallpaperAPI::INITIALIZED:
    {
      if (m_inputManager.IsKeyDown(Input::Key::Space))
      {
        m_appState = AppState::RUNNING;
        player.m_velocity.y = 6.0f;
      }
      break;
    }
    case WallpaperAPI::RUNNING:
    {
      glm::vec3 GRAVITY(0, -4.0f, 0);

      glm::vec3 velocity(GRAVITY);

      velocity += player.m_velocity;
      if (player.m_velocity.y > 0) {
        player.m_velocity += GRAVITY * delta;
      }
      else {
        player.m_velocity.y = 0;
      }

      player.m_position += (velocity * delta);

      if (m_inputManager.IsKeyDown(Input::Key::Space))
      {
        player.m_velocity.y = 6.0f;
      }
      break;
    }
    case WallpaperAPI::FAILED:
      break;
    default:
      break;
    }
    return;


    float offset = 1.0f / 60 * (m_inputManager.IsKeyDown(Input::Key::LeftShift) ? 10 : 1);

    if (m_inputManager.IsKeyDown(Input::Key::W))
    {
      player.m_position.y += offset;
    }
    if (m_inputManager.IsKeyDown(Input::Key::A))
    {
      player.m_position.x -= offset;
    }
    if (m_inputManager.IsKeyDown(Input::Key::S))
    {
      player.m_position.y -= offset;
    }
    if (m_inputManager.IsKeyDown(Input::Key::D))
    {
      player.m_position.x += offset;
    }


    if (m_inputManager.IsKeyDown(Input::Key::KPSubtract))
    {
      player.m_scale -= glm::vec3(offset, offset, offset);
    }
    if (m_inputManager.IsKeyDown(Input::Key::KPAdd))
    {
      player.m_scale += glm::vec3(offset, offset, offset);
    }
  }

  void Application::Render()
  {
    ShaderProgram& shader = m_renderer.GetShaderProgram();

    GL_CHECK(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
    GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    shader.Use();
    for (Entity& entity : m_entities)
    {
      glm::mat4 transform = glm::mat4(1.0f);
      transform = glm::translate(transform, entity.m_position);
      transform = glm::scale(transform, entity.m_scale);
      // transform = glm::rotate(transform, (float) glm::radians(m_rot), glm::vec3(0, 0, 1));

      shader.LoadMatrix4f("transform", glm::value_ptr(transform));

      // glm::vec3 out = glm::unProject(glm::vec3(0, 0, 0), transform * m_renderer.m_view, m_renderer.m_projection, glm::vec4(0, 0, 1920, 1080));
      // std::cout << "Out: " << out.x << ", " << out.y << ", " << out.z << std::endl;

      GL_CHECK(glActiveTexture(GL_TEXTURE0));
      GL_CHECK(glBindTexture(GL_TEXTURE_2D, entity.m_texture.textureId));

      GL_CHECK(glBindVertexArray(entity.m_VAO));

      GL_CHECK(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
    }

    m_renderer.RenderLine(m_line, glm::vec3(-1.22, -0.2, 0)); // It's center based?
    m_renderer.RenderLine(m_line, glm::vec3(0));

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
