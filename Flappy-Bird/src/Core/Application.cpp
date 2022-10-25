#include "pch.h"
#include "Application.h"
#define STB_IMAGE_IMPLEMENTATION
#include "Graphics/stb_image.h"
#include "MonitorManager.h"
#include "Entity/Entity.h"

namespace WallpaperAPI
{

  Application::Application()
    : m_desktopHWnd(Utils::GetWallpaperWindow()),
        m_desktopDC(GetDC(m_desktopHWnd)), m_renderer(Renderer(m_desktopHWnd, m_desktopDC)) {
    srand(time(NULL));
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

    MonitorManager monitorManager(m_desktopHWnd, m_desktopDC);

    if (monitorManager.GetMonitors().empty()) {
      std::cout << "No monitor available!" << std::endl;
      std::cin.get();
      return;
    }
    m_renderer.SetViewport(monitorManager.GetMonitors().at(0).area);

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
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_textureId);

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            m_renderer.SwapBuffers();
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
      const int width = 1920, height = 1080;
      glViewport(0, 0, width, height);
    }

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

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, model.vertices.size() * sizeof(Vertex), model.vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.vertices.size() * sizeof(glm::uvec3), model.indices.data(), GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // load and create a texture 
    // texture 1
    // ---------
    glGenTextures(1, &m_textureId);
    glBindTexture(GL_TEXTURE_2D, m_textureId);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char* data = stbi_load("resources/textures/background.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
      std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    // glUseProgram(shaderProgram); // don't forget to activate/use the shader before setting uniforms!

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
