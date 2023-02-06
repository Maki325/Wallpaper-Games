#include "pch.h"
#include "ScoreboardOverlay.h"
#include "Entity/Entity.h"
#include "Core/Loader.h"
#include "Utils/Utils.h"
#include "Core/Application.h"
#include "Graphics/Text.h"
#include "GameLayer.h"
#include <charconv>

static const std::string SCORE_FILE = "scores.txt";

namespace WallpaperAPI
{
  ScoreboardOverlay::ScoreboardOverlay()
    : Layer("ScoreboardOverlay"),
    m_backTexture("resources/textures/Back.png")
  {
  }

  void ScoreboardOverlay::OnAttach()
  {
    std::ifstream scores(SCORE_FILE);
    if (!scores.is_open())
    {
      std::ofstream create(SCORE_FILE);
      create << "";
      create.close();

      scores.open(SCORE_FILE);
    }
    std::string line;
    while (std::getline(scores, line))
    {
      if (line.empty()) continue;
      std::vector<std::string> splits = Utils::Split(line, "|");
      if (splits.size() != 3) continue;

      size_t score = 0;
      std::string &sscore = splits.at(0);
      std::from_chars(sscore.data(), sscore.data() + sscore.size(), score);

      m_scores.push_back(Score{ score, splits.at(1), splits.at(2), 0, 0 });
    }

    std::sort(m_scores.begin(), m_scores.end(), [](const Score& a, const Score& b) { return b.score < a.score; });

    CreateScores();
  }

  void ScoreboardOverlay::CreateScores()
  {
    if (m_scores.empty()) return;
    auto model = Loader::LoadObj("resources/models/pipe.obj");
    
    float offsetY = 0.8f / m_scores.at(0).score;
    float offsetX = 0;
    for (auto& score : m_scores)
    {
      score.x = offsetX;
      score.y = -1 + offsetY * score.score;
      m_pipes.push_back(Entity(model, glm::vec3(score.x, score.y, 1), glm::vec3(0, 0, 0), glm::vec3(0.25), glm::vec3(0), "resources/textures/pipe.png"));
      offsetX += 0.5f;
    }
  }

  void ScoreboardOverlay::OnDetach()
  {
  }

  void ScoreboardOverlay::OnUpdate(float delta)
  {
    auto &app = Application::GetApp();
    auto& im = app.GetInputManager();
    if (m_keyDown)
    {
      if (!im.IsKeyDown(Input::Key::Left) && !im.IsKeyDown(Input::Key::Up) && !im.IsKeyDown(Input::Key::Right) &&
        !im.IsKeyDown(Input::Key::Down))
      {
        m_keyDown = false;
      }
    }
    if (!m_keyDown) {
      if (im.IsKeyDown(Input::Key::Left))
      {
        auto changed = m_selected > 0;
        if (changed)
        {
          m_selected = m_selected - 1;

          for (auto& pipe : m_pipes)
          {
            pipe.m_position.x += 0.5f;
          }
          for (auto& score : m_scores)
          {
            score.x += 0.5;
          }
        }
        m_keyDown = true;

      }
      else if (im.IsKeyDown(Input::Key::Right))
      {
        auto changed = m_selected < m_scores.size() - 1;
        if (changed)
        {
          m_selected = m_selected + 1;

          for (auto& pipe : m_pipes)
          {
            pipe.m_position.x -= 0.5f;
          }
          for (auto& score : m_scores)
          {
            score.x -= 0.5;
          }
        }
        m_keyDown = true;
      }
    }

    auto& renderer = app.GetRenderer();
    if (im.IsMouseButtonDown(Input::MouseButton::ButtonLeft))
    {
      auto mousePos = im.GetMousePosition();
      mousePos.x -= renderer.GetViewport().x;
      mousePos.y -= renderer.GetViewport().y;

      auto halfWidth = m_backLocation.width / 2;
      auto diffX = mousePos.x - (m_backLocation.x + halfWidth);
      auto diffY = mousePos.y - (m_backLocation.y + halfWidth);

      if (diffX * diffX + diffY * diffY < halfWidth * halfWidth)
      {
        app.DisableLayer(GetID());
        app.EnableLayer(GameLayer::ID);
        // app.RemoveLayer(this);
        // app.AddLayer(new GameLayer());
      }
    }
    
    Render(delta);
  }

  Rect<float> &ScoreboardOverlay::backLocation(Renderer& renderer)
  {
    this->m_backLocation.x = 2 * renderer.vw();
    this->m_backLocation.y = 2 * renderer.vh();
    this->m_backLocation.width = this->m_backLocation.height = 4 * renderer.vw();

    return this->m_backLocation;
  }

  void ScoreboardOverlay::Render(float delta) {
    auto& renderer = Application::GetApp().GetRenderer();
    for (auto& pipe : m_pipes)
    {
      renderer.RenderEntity(pipe);
    }
    for (auto& score : m_scores)
    {
      int width = renderer.GetViewport().z;
      int height = renderer.GetViewport().w;
      // int x = (width / 2) * ((score.x) / 2) + (width / 2);

      glm::mat4 transform = glm::mat4(1.0f);
      transform = glm::translate(transform, glm::vec3(score.x /* - 0.5f * m_selected*/, score.y + 0.725f, 0));
      transform = glm::scale(transform, glm::vec3(1, 1, 1));
      auto position = ((renderer.m_projection * renderer.m_view) * transform) * glm::vec4(0, 0, 0, 1);
      // std::cout << "position: " << position.x << ", y: " << position.y << ", z: " << position.z << ", w: " << position.w << std::endl;

      int x = (width / 2) * ((position.x) / 2) + (width / 2);
      int y = (height / 2) * ((-(position.y)) / 2) + (height / 2);

      renderer.RenderText(Text(std::to_string(score.score), x - 5, y, glm::vec3(1.0f)).scale(0.6).centerHorizontal().shadow(true));
    }

    auto& rect = backLocation(renderer);
    renderer.RenderTexturedQuad(rect.x, rect.y, rect.width, rect.height, m_backTexture);

    if (!m_scores.empty()) {
      RenderSelectedScore();
    }
    else
    {
      RenderNoScores();
    }
  }

  void ScoreboardOverlay::RenderNoScores()
  {
    auto& renderer = Application::GetApp().GetRenderer();
    int x = 20 * renderer.vw();
    int y = 20 * renderer.vh();
    int width = 60 * renderer.vw();
    int height = 60 * renderer.vh();

    renderer.RenderColoredQuad(x, y, width, height, glm::vec4(0.91f, 1.0f, 0.55f, 1.0f));
    renderer.RenderColoredQuad(x + 10, y + 10, width - 20, height - 20, glm::vec4(0.61f, 0.88f, 0.35f, 1.0f));

    renderer.RenderText(
      Text("No records yet!", x + width / 2, y + height / 2)
      .centerHorizontal()
      .centerVertical()
    );
  }

  void ScoreboardOverlay::RenderSelectedScore()
  {
    auto& renderer = Application::GetApp().GetRenderer();

    auto& selectedScore = m_scores.at(m_selected);

    int x = 5 * renderer.vw();
    int y = 60 * renderer.vh();
    int width = 30 * renderer.vw();
    int height = 30 * renderer.vh();

    renderer.RenderColoredQuad(x, y, width, height, glm::vec4(0.91f, 1.0f, 0.55f, 1.0f));
    renderer.RenderColoredQuad(x + 10, y + 10, width - 20, height - 20, glm::vec4(0.61f, 0.88f, 0.35f, 1.0f));

    float textScale = 0.30f;

    int textY = y + 5 * renderer.vh();
    int textX = x + 20;

    const std::string& playerText = "Player:";
    const auto& player = Text(playerText, textX, textY, glm::vec3(1, 1, 1)).scale(textScale);

    int playerWidth = renderer.GetTextWidth(player);
    int playerHeight = renderer.GetTextHeight(player);

    renderer.RenderText(player);

    renderer.RenderText(
      Text(selectedScore.player, textX + playerWidth, textY, glm::vec3(1, 1, 1))
      .scale(textScale)
      .width(width - 20 - 10 - playerWidth)
      .overflow(Text::Ellipsis)
    );

    textY += playerHeight * 1.5;

    const std::string& playedOnText = "Played on:";
    const auto& playedOn = Text(playedOnText, textX, textY, glm::vec3(1, 1, 1)).scale(textScale);

    renderer.RenderText(playedOn);
    int playedOnWidth = renderer.GetTextWidth(playedOn);

    renderer.RenderText(
      Text(selectedScore.time, textX + playedOnWidth, textY, glm::vec3(1, 1, 1))
      .scale(textScale)
      .width(width - 20 - 10 - playedOnWidth)
      .overflow(Text::Ellipsis)
    );

  }

  void ScoreboardOverlay::OnImGuiRender()
  {
  }

  const std::string& ScoreboardOverlay::ID = "scoreboard_overlayer";
}
