#include "pch.h"
#include "Core/Application.h"
#include "Utils/Utils.h"

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR pCmdLine, _In_ int nCmdShow) {
  WallpaperAPI::Utils::InitConsole();

  WallpaperAPI::Application app;
  app.Run();

  return 0;
}
