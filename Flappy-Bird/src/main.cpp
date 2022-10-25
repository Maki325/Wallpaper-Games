#include "pch.h"
#include "Core/Application.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
  WallpaperAPI::Application app;
  app.Run();

  return 0;
}
