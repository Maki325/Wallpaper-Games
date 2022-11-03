#include "pch.h"
#include "ImGuiLayer.h"
#include <imgui.h>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace WallpaperAPI
{
  void GLFWMessageCallback(int code, const char* message)
  {
    std::cout << "Code: " << code << ", Message: " << message << std::endl;

    std::cin.get();
  }

  ImGuiLayer::ImGuiLayer()
    : Layer("ImGuiLayer")
  {
  }

  void ImGuiLayer::OnAttach()
  {
    glfwSetErrorCallback(GLFWMessageCallback);

    if (!glfwInit())
      throw std::runtime_error("Couldn't initialize GLFW!");

    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only

    m_window = glfwCreateWindow(640, 720, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
    if (m_window == NULL)
      throw std::runtime_error("Couldn't create GLFW window for ImGui!");

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();


    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init("#version 410");
  }

  void ImGuiLayer::OnDetach()
  {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(m_window);
    glfwTerminate();
  }

  bool ImGuiLayer::ShouldDetach()
  {
    return glfwWindowShouldClose(m_window);
  }

  void ImGuiLayer::Begin()
  {
    glfwMakeContextCurrent(m_window);
    glfwPollEvents();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
  }

  void ImGuiLayer::End()
  {
    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(m_window, &display_w, &display_h);
    GL_CHECK(glViewport(0, 0, display_w, display_h));
    GL_CHECK(glClearColor(0.2f, 0.3f, 0.9f, 1.0f));
    GL_CHECK(glClear(GL_COLOR_BUFFER_BIT));
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(m_window);
  }

  void ImGuiLayer::OnUpdate(float delta)
  {
  }
  void ImGuiLayer::OnImGuiRender()
  {
  }

}
