#include "pch.h"
#include "Application.h"
#define STB_IMAGE_IMPLEMENTATION
#include "Graphics/stb_image.h"
#include "../Graphics/VideoTask.h"
#include "MonitorManager.h"

namespace WallpaperAPI
{

  Application::Application()
    : m_desktopHWnd(Utils::GetWallpaperWindow()),
        m_desktopDC(GetDC(m_desktopHWnd)), m_openGL(OpenGL(m_desktopHWnd, m_desktopDC)) {
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
    m_openGL.SetViewport(monitorManager.GetMonitors().at(0).area);

    VideoTask videoTask("F:\\Wallpaper\\Wallpaper\\resources\\videos\\Test.mp4", -1);
    std::thread videoThread(&VideoTask::Run, &videoTask);

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
            VideoFrame* frame = (VideoFrame*) e.data;

            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            glBindTexture(GL_TEXTURE_2D, m_textureId);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame->width, frame->height, 0, GL_BGR, GL_UNSIGNED_BYTE, frame->data);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_textureId);

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            m_openGL.SwapBuffers();

            free(frame->data);
            free(frame);
            break;
          }
        }
      }
      s_mutex.unlock();
    }

    videoTask.Stop();
    videoThread.join();

    std::cout << "\nPress any key to exit...";
    std::cin.get();
    ResetWallpaper();
  }

  void Application::PostEvent(Event e) {
    s_mutex.lock();
    s_messages.push(e);
    s_mutex.unlock();
  }

  void GLAPIENTRY MessageCallback(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam
  ) {
    std::cout << "GL CALLBACK: " << (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "") << " type = " << type << ", severity = " << severity << ", message = " << message << std::endl;
  }

  bool Application::InitGraphics() {
    if (!m_openGL.IsInitialized()) return false;

    {
      const int width = 1920, height = 1080;
      glViewport(0, 0, width, height);
    }

    // During init, enable debug output
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);

    const char* vertexShaderSource =
      "#version 330 core\n"
      "layout (location = 0) in vec3 aPos;\n"
      "layout (location = 1) in vec3 aColor;\n"
      "layout (location = 2) in vec2 aTexCoord;\n"
      "\n"
      "out vec3 ourColor;\n"
      "out vec2 TexCoord;\n"
      "\n"
      "void main()\n"
      "{\n"
      "	gl_Position = vec4(aPos, 1.0);\n"
      "	ourColor = aColor;\n"
      "	TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
      "}\n";
    const char* fragmentShaderSource =
      "#version 330 core\n"
      "out vec4 FragColor;\n"
      "\n"
      "in vec3 ourColor;\n"
      "in vec2 TexCoord;\n"
      "\n"
      "// texture samplers\n"
      "uniform sampler2D texture1;\n"
      "\n"
      "void main()\n"
      "{\n"
      "// linearly interpolate between both textures (80% container, 20% awesomeface)\n"
      "FragColor = texture(texture1, TexCoord);\n"
      "}\n";

    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
      glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
      std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
      glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
      std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
      std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    /*float vertices[] = {
          0.5f,  0.5f, 0.0f,  // top right
          0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left
    };*/
    float vertices[] = {
      // positions          // colors           // texture coords
        1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
        1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
      -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
      -1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

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
    glUseProgram(shaderProgram); // don't forget to activate/use the shader before setting uniforms!

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
