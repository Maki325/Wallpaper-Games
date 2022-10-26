#include "pch.h"
#include "Application.h"
#include "Entity/Entity.h"

namespace WallpaperAPI
{

  Application::Application()
    : m_desktopHWnd(Utils::GetWallpaperWindow()),
        m_desktopDC(GetDC(m_desktopHWnd)), m_renderer(Renderer(m_desktopHWnd, m_desktopDC)),
        m_monitorManager(MonitorManager(m_desktopHWnd, m_desktopDC)) {
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
    if (!InitGraphics()) {
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
    m_renderer.SetViewport(m_monitorManager.GetMonitors().at(0).area);

    s_mutex.lock();
    Event e{ Event::Type::EVENT_TYPE_RENDER, NULL };
    s_messages.push(e);
    Event e1{ Event::Type::EVENT_TYPE_QUIT, NULL };
    s_messages.push(e1);
    s_mutex.unlock();

    while (m_running) {
      s_mutex.lock();
      while (!this->s_messages.empty()) {
        Event e = this->s_messages.front();
        this->s_messages.pop();
        switch (e.type) {
          case Event::Type::EVENT_TYPE_QUIT: {
            m_running = false;
            break;
          }
          case Event::Type::EVENT_TYPE_RESET: {
            ResetWallpaper();
            break;
          }
          case Event::Type::EVENT_TYPE_RENDER: {
            RECT area = m_monitorManager.GetMonitors().at(0).area;
            Utils::PrintRect(area);

            int monitorWidth = abs(area.right - area.left);
            int monitorHeight = abs(area.bottom - area.top);

            std::cout << "monitorWidth: " << monitorWidth << " monitorHeight: " << monitorHeight << "\n";

            glm::mat4 projection = glm::mat4(1.0f);
            projection = glm::perspective(glm::radians(45.0f), (float)monitorWidth / (float)monitorHeight, 0.01f, 1000.0f);
            glm::mat4 view = glm::mat4(1.0f);
            view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

            glm::mat4 transform = glm::mat4(1.0f);
            transform = glm::translate(transform, glm::vec3(0, 0, 0));

            ShaderProgram& shader = m_renderer.GetShaderProgram();
            shader.Use();

            shader.LoadMatrix4f("projection", glm::value_ptr(projection));
            shader.LoadMatrix4f("view", glm::value_ptr(view));

            for (const Entity& entity : m_entities)
            {
              for (size_t i = 0; i < 100; i++) {
                transform = glm::translate(transform, glm::vec3(0.005f, 0, 0));
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
            break;
          }
        }
      }
      s_mutex.unlock();
    }

    std::cout << "\nPress any key to exit...";
    std::cin.get();
    ResetWallpaper();
  }

  void Application::PostEvent(Event e) {
    s_mutex.lock();
    s_messages.push(e);
    s_mutex.unlock();
  }

  bool Application::InitGraphics() {
    if (!m_renderer.IsInitialized()) return false;

    {
      RECT &area = m_monitorManager.GetMonitors().at(0).area;

      int width = abs(area.right - area.left);
      int height = abs(area.bottom - area.top);
      glViewport(0, 0, width, height);
    }

    return true;
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
