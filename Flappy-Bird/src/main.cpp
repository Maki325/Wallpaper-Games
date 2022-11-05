#include "pch.h"
#include "Core/Application.h"
#include "Utils/Utils.h"
#include "Windows.h"
#include <CommCtrl.h>
#include "imgui_impl_win32.h"
#include "imgui_impl_opengl3.h"
#include "Core/ImGuiBackend.h"

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR pCmdLine, _In_ int nCmdShow) {
  WallpaperAPI::Utils::InitConsole();

  WallpaperAPI::Application app;
  app.Run();

  return 0;

  /*WNDCLASS wc = {0};
  wc.lpfnWndProc = WndProc;
  wc.hInstance = hInstance;
  wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
  wc.lpszClassName = (LPCWSTR)"NCUI";
  wc.style = CS_OWNDC;
  if (!RegisterClass(&wc))
    return 1;*/

  HWND g_hwnd = WallpaperAPI::Utils::GetWallpaperWindow();

  HDC m_desktopDC = GetDC(g_hwnd);

  // Show the window
  ShowWindow(g_hwnd, SW_SHOWDEFAULT);
  UpdateWindow(g_hwnd);


  // https://www.khronos.org/opengl/wiki/Creating_an_OpenGL_Context_%28WGL%29
  PIXELFORMATDESCRIPTOR pfd =
  {
    sizeof(PIXELFORMATDESCRIPTOR),
    1,
    PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    //Flags
    PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
    32,                   // Colordepth of the framebuffer.
    0, 0, 0, 0, 0, 0,
    0,
    0,
    0,
    0, 0, 0, 0,
    24,                   // Number of bits for the depthbuffer
    8,                    // Number of bits for the stencilbuffer
    0,                    // Number of Aux buffers in the framebuffer.
    PFD_MAIN_PLANE,
    0,
    0, 0, 0
  };

  int letWindowsChooseThisPixelFormat = ChoosePixelFormat(m_desktopDC, &pfd);
  SetPixelFormat(m_desktopDC, letWindowsChooseThisPixelFormat, &pfd);

  HGLRC m_openGLRenderingContext = wglCreateContext(m_desktopDC);
  wglMakeCurrent(m_desktopDC, m_openGLRenderingContext);

  if (!gladLoadGL()) {
    throw std::exception("Couldn't load Glad!");
  }

  // Setup Dear ImGui binding
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;

  //Init Win32
  ImGui_ImplWin32_Init(g_hwnd);

  //Init OpenGL Imgui Implementation
  // GL 3.0 + GLSL 130
  const char* glsl_version = "#version 130";
  ImGui_ImplOpenGL3_Init(glsl_version);

  //Set Window bg color
  ImVec4 clear_color = ImVec4(0.2f, 0.3f, 0.9f, 1.0f);

  // Setup style
  ImGui::StyleColorsClassic();

  // Main loop
  MSG msg;
  ZeroMemory(&msg, sizeof(msg));
  while (msg.message != WM_QUIT)
  {
    // Poll and handle messages (inputs, window resize, etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
    // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
    if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
      continue;
    }

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplWin32_NewFrame();
    POINT p = { 0, 0 };
    GetCursorPos(&p);
    io.MousePos = ImVec2((float)p.x, (float)p.y);
    ImGui::NewFrame();

    WallpaperAPI::ImGuiBackend::Update();

    //show Main Window
    ImGui::ShowDemoWindow();

    if (ImGui::Button("Test"))
    {
      std::cout << "WORKS" << std::endl;
    }

    // Rendering
    ImGui::Render();
    wglMakeCurrent(m_desktopDC, m_openGLRenderingContext);
    glViewport(0, 0, 3 * 1920, 1080);                 //Display Size got from Resize Command
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    wglMakeCurrent(m_desktopDC, m_openGLRenderingContext);
    SwapBuffers(m_desktopDC);
    // std::cout << "Render" << std::endl;
  }

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  wglDeleteContext(m_openGLRenderingContext);
  ImGui::DestroyContext();
  ImGui_ImplWin32_Shutdown();

  DestroyWindow(g_hwnd);
  // UnregisterClass((LPCWSTR)"NCUI", wc.hInstance);

  // HWND g_hwnd = CreateWindow(wc.lpszClassName, (LPCWSTR) "teste", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0, 640, 480, 0, 0, hInstance, 0);


  // WallpaperAPI::Application app;
  // app.Run();

  return 0;
}
