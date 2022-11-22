#pragma once
#include "Core/Layer.h"
#include "Entity/Entity.h"
#include "Colliders/AABB.h"
#include "Colliders/CircleCollider.h"
#include "Game/Obstacle.h"

namespace WallpaperAPI
{
  enum GameState
  {
    INITIALIZED,
    RUNNING,
    FAILED
  };

  class GameLayer : public Layer
  {
  public:
    GameLayer();

    void OnAttach() override;
    void OnDetach() override;

    void OnUpdate(float delta) override;
    void OnImGuiRender() override;

  private:
    void SetInitial();

    void Update(float delta);
    void UpdateInitialized(float delta);
    void UpdateRunning(float delta);
    void UpdateFailed(float delta);
    void ScrollGround(float delta);
    float GetHeight();
    void ScrollPipes(float delta);
    void Render();

    void RenderButton(float x, float y, float width, float height, const std::string& text, glm::vec3& textColor, float textScale = 1.0f);

  private:
    bool m_running = true;
    Entity m_player;
    Obstacle m_obstacles[11];
    size_t m_score = 0;
    Texture m_buttonTexture;

    std::vector<Entity> m_ground;
    AABB m_groundAABB;
    CircleCollider m_playerCollider;

    GameState m_gameState = GameState::INITIALIZED;
  };
}

