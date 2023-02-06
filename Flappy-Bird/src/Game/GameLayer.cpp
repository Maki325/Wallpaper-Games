#include "pch.h"
#include "GameLayer.h"
#include "Core/Application.h"
#include "Core/Loader.h"
#include "Graphics/Renderer.h"
#include "Colliders/Collider.h"
#include <imgui.h>
#include "Graphics/Text.h"
#include "Core/SystemTray/Components/SyncedCheckbox.h"
#include "ScoreboardOverlay.h"

namespace WallpaperAPI
{
  GameLayer::GameLayer()
    : Layer("GameLayer"),
    m_player({
          {
          // positions              // texture coords
          { { 1.0f,  1.0f, 0.0f},   {1.0f, 1.0f} }, // top right
          { { 1.0f, -1.0f, 0.0f},   {1.0f, 0.0f} }, // bottom right
          { {-1.0f, -1.0f, 0.0f},   {0.0f, 0.0f} }, // bottom left
          { {-1.0f,  1.0f, 0.0f},   {0.0f, 1.0f} }, // top left
        }, {  // note that we start from 0!
          { 0, 1, 3 }, // first Triangle
          { 1, 2, 3 }  // second Triangle
        }
      }, glm::vec3(0, 0, 0), glm::vec3(0), glm::vec3(0.25), glm::vec3(0), "resources/textures/flappy-bird.png"),
    m_groundAABB(glm::vec2(0.0f, -1.0f), glm::vec2(10.0f, 0.24f)),
    m_playerCollider(glm::vec2(0.0f, 0.0f), 0.10f),
    m_buttonTexture("resources/textures/button.png"),
    m_pausedOverlay(new PausedOverlay())
  {
  }

  void GameLayer::OnAttach()
  {
    auto &app = Application::GetApp();
    app.GetRenderer().MakeContextCurrent();
    Model model = Loader::LoadObj("resources/models/ground.obj");

    m_ground.emplace_back(model, glm::vec3(-1.22, -1, 0), glm::vec3(0), glm::vec3(0.25f), glm::vec3(0), "resources/textures/ground.png");
    m_ground.emplace_back(model, glm::vec3( 0.50, -1, 0), glm::vec3(0), glm::vec3(0.25f), glm::vec3(0), "resources/textures/ground.png");
    m_ground.emplace_back(model, glm::vec3( 2.22, -1, 0), glm::vec3(0), glm::vec3(0.25f), glm::vec3(0), "resources/textures/ground.png");
    m_ground.emplace_back(model, glm::vec3( 3.94, -1, 0), glm::vec3(0), glm::vec3(0.25f), glm::vec3(0), "resources/textures/ground.png");

    app.GetSystemTray().AddComponent(
      SystemTray::SyncedCheckbox(
        "Running",
        m_running
      )
    );

    m_menuOpenedCallback = [=] { this->PauseGame(); };
    app.GetSystemTray().AddMenuOpenListener(&m_menuOpenedCallback);

    SetInitial();
  }

  void GameLayer::SetInitial()
  {
    m_player.m_position.y = m_playerCollider.m_position.y = 0;

    int pos = 3;
    for (auto& obstacle : m_obstacles)
    {
      obstacle.SetPosition(glm::vec3(pos++, GetHeight(), 0));
    }

    m_score = 0;

    m_gameState = GameState::INITIALIZED;
  }
  
  void GameLayer::OnDetach()
  {
    auto& app = Application::GetApp();
    app.GetSystemTray().RemoveMenuOpenListener(&m_menuOpenedCallback);
  }

  void GameLayer::OnUpdate(float delta)
  {
    Application::GetApp().GetRenderer().MakeContextCurrent();
    if (m_running)
    {
      Update(delta);
    }
    else
    {
      UpdatePaused(delta);
    }
    Render();
  }

  void GameLayer::OnImGuiRender()
  {
    ImGui::Begin("Flappy Bird");

    ImGui::DragFloat3("Position", glm::value_ptr(m_player.m_position), 0.001f);

    ImGui::Text("GameState");
    ImGui::RadioButton("Initialized", (int*)&m_gameState, GameState::INITIALIZED); ImGui::SameLine();
    ImGui::RadioButton("Running", (int*)&m_gameState, GameState::RUNNING); ImGui::SameLine();
    ImGui::RadioButton("Failed", (int*)&m_gameState, GameState::FAILED);

    ImGui::End();
  }

  void GameLayer::Update(float delta)
  {
    if (Application::GetApp().GetInputManager().IsKeyDown(Input::Key::R))
    {
      SetInitial();
    }
    switch (m_gameState)
    {
    case GameState::INITIALIZED:
      UpdateInitialized(delta);
      break;
    case GameState::RUNNING:
      UpdateRunning(delta);
      break;
    case GameState::FAILED:
      UpdateFailed(delta);
      break;
    default:
      break;
    }
  }

  void GameLayer::PauseGame()
  {
    m_running = false;
    Application::GetApp().GetRenderer().MakeContextCurrent();

    Application::GetApp().AddLayer(m_pausedOverlay);

    Render();
    Application::GetApp().GetRenderer().SwapBuffers();

    m_pausedOverlay->OnUpdate(0);

    Application::GetApp().GetRenderer().SwapBuffers();

  }

  void GameLayer::UpdateInitialized(float delta)
  {
    ScrollGround(delta);
    if (Application::GetApp().GetInputManager().IsKeyDown(Input::Key::Space))
    {
      m_gameState = GameState::RUNNING;
      m_player.m_velocity.y = 6.0f;
    }
  }

  void GameLayer::UpdateRunning(float delta)
  {
    ScrollGround(delta);
    ScrollPipes(delta);
    glm::vec3 GRAVITY(0, -4.0f, 0);

    glm::vec3 velocity(GRAVITY);

    velocity += m_player.m_velocity;
    if (m_player.m_velocity.y > 0) {
      m_player.m_velocity += GRAVITY * delta;
    }
    else {
      m_player.m_velocity.y = 0;
    }

    m_player.m_position += (velocity * delta);

    if (Application::GetApp().GetInputManager().IsKeyDown(Input::Key::Space))
    {
      m_player.m_velocity.y = 6.0f;
    }

    m_playerCollider.m_position.y = m_player.m_position.y;

     if (Collider::IsColliding(m_playerCollider, m_groundAABB))
     {
       m_gameState = GameState::FAILED;
     }

    for (auto& obstacle : m_obstacles)
    {
      if (obstacle.IsColliding(m_playerCollider))
      {
        m_gameState = GameState::FAILED;
        break;
      }
    }
  }

  void GameLayer::UpdateFailed(float delta)
  {
    Application& app = Application::GetApp();
    Renderer& renderer = app.GetRenderer();
    InputManager &inputManager = app.GetInputManager();
    auto pos = inputManager.GetMousePosition();

    int startX = renderer.GetViewport().x;
    int startY = renderer.GetViewport().y;

    int width = renderer.GetViewport().z;
    int height = renderer.GetViewport().w;
    float widthHalf = width / 2.0f;

    if (!inputManager.IsMouseButtonDown(Input::MouseButton::ButtonLeft))
    {
      return;
    }
    // widthHalf - 250, height / 2.0f, 500, 80
    if (pos.x - startX >= widthHalf - 250 && pos.x - startX <= widthHalf + 250 &&
        pos.y - startY >= height / 2.0f && pos.y - startY <= height / 2.0f + 80)
    {
      SetInitial();
      return;
    }

    // widthHalf - 250, height / 2.0f + 90, 245, 60
    if (pos.x - startX >= widthHalf - 250 && pos.x - startX <= widthHalf - 5 &&
      pos.y - startY >= height / 2.0f + 90 && pos.y - startY <= height / 2.0f + 90 + 60)
    {
      app.DisableLayer(GetID());
      app.EnableLayer(ScoreboardOverlay::ID);
      // app.RemoveLayer(this);
      // app.AddLayer(new ScoreboardOverlay());
      return;
    }

    // widthHalf + 5, height / 2.0f + 90, 245, 60
    if (pos.x - startX >= widthHalf + 5 && pos.x - startX <= widthHalf + 245 &&
      pos.y - startY >= height / 2.0f + 90 && pos.y - startY <= height / 2.0f + 90 + 60)
    {
      app.Exit();
      return;
    }
  }

  void GameLayer::UpdatePaused(float delta)
  {
    if (Application::GetApp().GetInputManager().IsKeyDown(Input::Key::Space))
    {
      m_running = true;
      Application::GetApp().RemoveLayer(m_pausedOverlay);
    }
  }

  void GameLayer::ScrollGround(float delta)
  {
    for (Entity& ground : m_ground)
    {
      ground.m_position.x -= 1.0f * delta;

      if (ground.m_position.x <= -1.72f * 1.75f) {
        ground.m_position.x += 1.72f * 4;
      }
    }
  }

  float GameLayer::GetHeight()
  {
    return (rand() % 100) / 200.0f;
  }

  void GameLayer::ScrollPipes(float delta)
  {
    for (auto& obstacle : m_obstacles)
    {
      auto& position = obstacle.GetPosition();

      bool test = false;
      if (position.x >= 0)
      {
        test = true;
      }
      position.x -= 1.0f * delta;
      if (test && position.x <= 0)
      {
        m_score++;
      }

      if (position.x <= -4) {
        position.x += 11;
        position.y = GetHeight();
      }

      obstacle.SetPosition(position);
    }
  }

  void GameLayer::Render()
  {
    Application& app = Application::GetApp();
    Renderer& renderer = app.GetRenderer();
    ShaderProgram& shader = renderer.GetShaderProgram();

    renderer.RenderEntity(m_player);

    for (auto& obstacle : m_obstacles)
    {
      obstacle.Render(renderer);
    }
    for (auto &entity : m_ground)
    {
      renderer.RenderEntity(entity);
    }

    std::string score = std::to_string(m_score);

    int width = renderer.GetViewport().z;
    int height = renderer.GetViewport().w;
    float widthHalf = width / 2.0f;

    renderer.RenderText(Text(score, widthHalf, 100.0f, glm::vec3(1, 1, 1)).centerHorizontal().shadow(true));

    if (m_gameState == GameState::FAILED)
    {
      // 100 height - 0.5f font scale
      RenderButton(widthHalf - 250, height / 2.0f, 500, 80, Text("Restart", 0, 0, glm::vec3(1, 1, 1)).scale(0.4f));

      RenderButton(widthHalf - 250, height / 2.0f + 90, 245, 60, Text("Scoreboard", 0, 0, glm::vec3(1, 1, 1)).scale(0.30f));
      RenderButton(widthHalf + 5, height / 2.0f + 90, 245, 60, Text("Exit", 0, 0, glm::vec3(1, 1, 1)).scale(0.30f));
    }
  }

  void GameLayer::RenderButton(float x, float y, float width, float height, Text& text)
  {
    Application& app = Application::GetApp();
    Renderer& renderer = app.GetRenderer();

    renderer.RenderColoredQuad(x, y, width, height, glm::vec4(0.91f, 1.0f, 0.55f, 1.0f));
    renderer.RenderColoredQuad(x + 10, y + 10, width - 20, height - 20, glm::vec4(0.61f, 0.88f, 0.35f, 1.0f));

    int textHeight = renderer.GetTextHeight(text);
    renderer.RenderText(text.x(x + width / 2.0f).y(y + textHeight / 1.8f).centerHorizontal());
  }

  const std::string& GameLayer::ID = "game_layer";
}
