#include "pch.h"
#include "PausedOverlay.h"
#include "Entity/Entity.h"
#include "Core/Loader.h"
#include "Utils/Utils.h"
#include "Core/Application.h"
#include <charconv>

const std::string SCORE_FILE = "scores.txt";

namespace WallpaperAPI
{
  PausedOverlay::PausedOverlay()
    : Layer("PausedOverlay")
  {
  }

  void PausedOverlay::OnAttach()
  {
  }

  void PausedOverlay::OnDetach()
  {
  }

  void PausedOverlay::OnUpdate(float delta)
  {
    Application& app = Application::GetApp();
    Renderer& renderer = app.GetRenderer();

    int width = renderer.GetViewport().z;
    int height = renderer.GetViewport().w;
    float widthHalf = width / 2.0f;

    const std::string pausedText = "PAUSED";
    auto& paused = Text(pausedText, widthHalf, 0, glm::vec3(1, 1, 1)).centerHorizontal().shadow(true);
    float textHeight = renderer.GetTextHeight(paused);
    paused.y((height - textHeight) / 2.0f);
    renderer.RenderColoredQuad(0, 0, width, height, glm::vec4(0.0f, 0.0f, 0.0f, 0.25f));
    renderer.RenderText(paused);

    renderer.RenderText(Text("Press space to unpause!", widthHalf, (height - textHeight) / 2.0f + 150, glm::vec3(1, 1, 1)).scale(0.25f).centerHorizontal().shadow(true));
  }

  void PausedOverlay::OnImGuiRender()
  {
  }

  const std::string& PausedOverlay::ID = "paused_layer";
}
