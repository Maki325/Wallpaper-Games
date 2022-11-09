#include "pch.h"
#include "GameLayer.h"
#include "Core/Application.h"
#include "Core/Loader.h"
#include "Graphics/Renderer.h"
#include "imgui.h"

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
      }, glm::vec3(-1, 0, 0), glm::vec3(0), glm::vec3(0.25), glm::vec3(0), "resources/textures/flappy-bird.png")
  {
  }

  void GameLayer::OnAttach()
  {
    Application::GetApp().GetRenderer().MakeContextCurrent();
    Model model = Loader::LoadObj("resources/models/ground.obj");

    m_ground.emplace_back(model, glm::vec3(-1.22, -1, 0), glm::vec3(0), glm::vec3(0.25f), glm::vec3(0), "resources/textures/Ground.png");
    m_ground.emplace_back(model, glm::vec3( 0.50, -1, 0), glm::vec3(0), glm::vec3(0.25f), glm::vec3(0), "resources/textures/Ground.png");
    m_ground.emplace_back(model, glm::vec3( 2.22, -1, 0), glm::vec3(0), glm::vec3(0.25f), glm::vec3(0), "resources/textures/Ground.png");
    m_ground.emplace_back(model, glm::vec3( 3.94, -1, 0), glm::vec3(0), glm::vec3(0.25f), glm::vec3(0), "resources/textures/Ground.png");

    m_aabbs.emplace_back(glm::vec2(-1.0f,  0.0f), glm::vec2(0.14f, 0.10f));
    m_aabbs.emplace_back(glm::vec2( 0.0f, -1.0f), glm::vec2(10.0f, 0.24f));
  }
  
  void GameLayer::OnDetach()
  {
  }

  void GameLayer::OnUpdate(float delta)
  {
    Application::GetApp().GetRenderer().MakeContextCurrent();
    Update(delta);
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
    switch (m_gameState)
    {
    case GameState::INITIALIZED:
      UpdateInitialized(delta);
      break;
    case GameState::RUNNING:
      UpdateRunning(delta);
      break;
    case GameState::FAILED:
      break;
    default:
      break;
    }
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

    AABB& playerAABB = m_aabbs.at(0);
    playerAABB.m_position.y = m_player.m_position.y;

    if (AABB::IsColliding(playerAABB, m_aabbs.at(1)))
    {
      m_gameState = GameState::FAILED;
    }
  }

  void GameLayer::UpdateFailed(float delta)
  {
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

  void GameLayer::Render()
  {
    Application& app = Application::GetApp();
    Renderer& renderer = app.GetRenderer();
    ShaderProgram& shader = renderer.GetShaderProgram();

    GL_CHECK(glClearColor(0.2f, 0.3f, 0.9f, 1.0f));
    GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    shader.Use();
    renderer.RenderEntity(m_player);
    for (Entity& entity : m_ground)
    {
      renderer.RenderEntity(entity);
    }
  }
}
