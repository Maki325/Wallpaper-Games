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
    void Update(float delta);
    void UpdateInitialized(float delta);
    void UpdateRunning(float delta);
    void UpdateFailed(float delta);
    void ScrollGround(float delta);
    float GetHeight();
    void ScrollPipes(float delta);
    void Render();

  private:
    size_t m_score;
    Entity m_player;
    Obstacle m_obstacles[11];

    std::vector<Entity> m_ground;
    AABB m_groundAABB;
    CircleCollider m_playerCollider;

    GameState m_gameState = GameState::INITIALIZED;
  };
}

