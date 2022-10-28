#include "pch.h"
#include "Application.h"
#include "Entity/Entity.h"

namespace WallpaperAPI
{

  Application::Application()
    : m_desktopHWnd(Utils::GetWallpaperWindow()),
        m_desktopDC(GetDC(m_desktopHWnd)), m_renderer(Renderer(m_desktopHWnd, m_desktopDC)),
        m_monitorManager(MonitorManager(m_desktopHWnd, m_desktopDC)), m_inputManager(m_desktopHWnd) {
    srand(time(NULL));

    Vertex vertices[] = {
      // positions          // texture coords
      { { 0.5f,  0.5f, 0.0f},   {1.0f, 1.0f} }, // top right
      { { 0.5f, -0.5f, 0.0f},   {1.0f, 0.0f} }, // bottom right
      { {-0.5f, -0.5f, 0.0f},   {0.0f, 0.0f} }, // bottom left
      { {-0.5f,  0.5f, 0.0f},   {0.0f, 1.0f} }, // top left
    };
    glm::uvec3 indices[] = {  // note that we start from 0!
      { 0, 1, 3 }, // first Triangle
      { 1, 2, 3 }  // second Triangle
    };

    std::vector<Vertex> verticesVec(vertices, vertices + 4);
    std::vector<glm::uvec3> indicesVec(indices, indices + 2);
    Model model{ verticesVec , indicesVec };
    m_entities.emplace_back(model, glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), "resources/textures/background.jpg");
  }

  Application::~Application() {
    s_mutex.lock();
    s_messages.swap(std::queue<Event>());
    s_mutex.unlock();
  }

  void Application::Run() {
    ConsoleTask::InitConsole();
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

    {
      ShaderProgram& shader = m_renderer.GetShaderProgram();
      RECT area = m_monitorManager.GetMonitors().at(0).area;
      int monitorWidth = abs(area.right - area.left);
      int monitorHeight = abs(area.bottom - area.top);

      std::cout << "monitorWidth: " << monitorWidth << " monitorHeight: " << monitorHeight << "\n";

      glm::mat4 projection = glm::mat4(1.0f);
      projection = glm::perspective(glm::radians(45.0f), (float)monitorWidth / (float)monitorHeight, 0.01f, 1000.0f);
      glm::mat4 view = glm::mat4(1.0f);
      view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

      shader.Use();

      shader.LoadMatrix4f("projection", glm::value_ptr(projection));
      shader.LoadMatrix4f("view", glm::value_ptr(view));
    }

    std::chrono::milliseconds previous = Utils::GetMillis();
    std::chrono::milliseconds start = previous;
    std::chrono::milliseconds lag(0);
    const std::chrono::milliseconds MS_PER_UPDATE(16);
    const std::chrono::milliseconds sec(10 * 1000);

    while (Utils::GetMillis() < start + sec)
    {
      std::chrono::milliseconds current = Utils::GetMillis();
      std::chrono::milliseconds elapsed = current - previous;
      previous = current;
      lag += elapsed;

      ProcessInput();

      while (lag >= MS_PER_UPDATE)
      {
        Update();
        lag -= MS_PER_UPDATE;
      }

      Render();
    }

    std::cout << "\nPress any key to exit...";
    std::cin.get();
    ResetWallpaper();
  }

  glm::vec2 GetProjectedPoint(glm::vec4 vec, glm::mat4 mat)
  {
    glm::vec4 point = vec * mat;

    // RECT area = m_monitorManager.GetMonitors().at(0).area;
    // int monitorWidth = abs(area.right - area.left);
    // int monitorHeight = abs(area.bottom - area.top);

    double px = point.x / point.z * 1920;
    double py = point.y / point.z * 1080;

    return glm::vec2(px, py);
  }

  glm::vec3 Temp(glm::vec3 pos, glm::mat4 modelview, glm::mat4 projection, glm::mat4 viewport)
  {
    glm::mat4 A = projection * modelview;
    glm::mat4 m = glm::inverse(A);

    glm::vec4 in(0);
    in[0] = (pos.x - (float)viewport[0][0]) / (float)viewport[1][0] * 2.0 - 1.0;
    in[1] = (pos.y - (float)viewport[0][1]) / (float)viewport[1][1] * 2.0 - 1.0;
    in[2] = 2.0 * pos.z - 1.0;
    in[3] = 1.0;

    glm::vec4 out(0);
    out = m * in;

    if (out[3] == 0.0) {
      return glm::vec3(0, 0, 0);
    }
    out[3] = 1.0 / out[3];

    return glm::vec3(out[0] * out[3], out[1] * out[3], out[2] * out[3]);
  }

  /*int glhUnProjectf(float winx, float winy, float winz, float* modelview, float* projection, int* viewport, float* objectCoordinate)
  {
    // Transformation matrices
    float m[16], A[16];
    float in[4], out[4];
    // Calculation for inverting a matrix, compute projection x modelview
    // and store in A[16]
    MultiplyMatrices4by4OpenGL_FLOAT(A, projection, modelview);
    // Now compute the inverse of matrix A
    if (glhInvertMatrixf2(A, m) == 0)
      return 0;
    // Transformation of normalized coordinates between -1 and 1
    in[0] = (winx - (float)viewport[0]) / (float)viewport[2] * 2.0 - 1.0;
    in[1] = (winy - (float)viewport[1]) / (float)viewport[3] * 2.0 - 1.0;
    in[2] = 2.0 * winz - 1.0;
    in[3] = 1.0;
    // Objects coordinates
    MultiplyMatrixByVector4by4OpenGL_FLOAT(out, m, in);
    if (out[3] == 0.0)
      return 0;
    out[3] = 1.0 / out[3];
    objectCoordinate[0] = out[0] * out[3];
    objectCoordinate[1] = out[1] * out[3];
    objectCoordinate[2] = out[2] * out[3];
    return 1;
  }*/

  // SCREEN SPACE: mouse_x and mouse_y are screen space
  glm::vec3 ViewToWorldCoordTransform(int mouse_x, int mouse_y, glm::mat4 GlobalProjection, glm::mat4 GlobalView) {
    // NORMALISED DEVICE SPACE
    double x = 2.0 * mouse_x / 1920 - 1;
    double y = 2.0 * mouse_y / 1080 - 1;
    // HOMOGENEOUS SPACE
    glm::vec4 screenPos = glm::vec4(x, -y, -1.0f, 1.0f);
    // Projection/Eye Space
    glm::mat4 ProjectView = GlobalProjection * GlobalView;
    glm::mat4 viewProjectionInverse = inverse(ProjectView);
    glm::vec4 worldPos = viewProjectionInverse * screenPos;
    return glm::vec3(worldPos);
  }

  void Application::ProcessInput()
  {
    Entity &player = m_entities.at(0);

    auto pos = m_inputManager.GetMousePosition();

    // player.m_position.x = (pos.x - 1920) / (1920.0f / 2) - 1;
    // player.m_position.y = -1 * (pos.y / (1080.0f / 2) - 1);

    std::cout << "x: " << pos.x - 1920 << ", y: " << pos.y << std::endl;

    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, player.m_position);


    RECT area = m_monitorManager.GetMonitors().at(0).area;
    int monitorWidth = abs(area.right - area.left);
    int monitorHeight = abs(area.bottom - area.top);

    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(45.0f), (float)monitorWidth / (float)monitorHeight, 0.01f, 1000.0f);
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

    auto calc = Temp(glm::vec3(pos, 0), transform * view, projection, view);

    // std::cout << "calc x: " << calc.x << ", y: " << calc.y << std::endl;

    // player.m_position.x = calc.x;
    // player.m_position.y = calc.y;

    auto calc2 = ViewToWorldCoordTransform(
      pos.x - 1920, pos.y, projection, view);

    std::cout << "calc2 x: " << calc2.x << ", y: " << calc2.y << std::endl;

    player.m_position.x = calc2.x * 1;
    player.m_position.y = calc2.y * 1;

    return;

    if (GetKeyState('W') & 0x8000)
    {
      player.m_position.y += 1.0f / 60;
    }
    if (GetKeyState('A') & 0x8000)
    {
      player.m_position.x -= 1.0f / 60;
    }
    if (GetKeyState('S') & 0x8000)
    {
      player.m_position.y -= 1.0f / 60;
    }
    if (GetKeyState('D') & 0x8000)
    {
      player.m_position.x += 1.0f / 60;
    }
  }

  void Application::Update()
  {
    /*for (Entity& entity : m_entities)
    {
      entity.m_position.x += 1.0f / 60;
    }*/
  }

  void Application::Render()
  {
    ShaderProgram& shader = m_renderer.GetShaderProgram();

    for (Entity& entity : m_entities)
    {
      glm::mat4 transform = glm::mat4(1.0f);
      transform = glm::translate(transform, entity.m_position);
      shader.LoadMatrix4f("transform", glm::value_ptr(transform));
      glBindVertexArray(entity.m_VAO);

      glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, entity.m_texture.textureId);

      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

      m_renderer.SwapBuffers();
    }
  }

  void Application::PostEvent(Event e) {
    s_mutex.lock();
    s_messages.push(e);
    s_mutex.unlock();
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

  std::queue<Event> Application::s_messages;
  std::mutex Application::s_mutex;
}
