#pragma once
#include "Core/Layer.h"
#include "Entity/Entity.h"
#include "Entity/Line.h"
#include "Core/AABB.h"

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
    void Render();

  private:
    Line m_line;
    Entity m_player;

    std::vector<Entity> m_ground;
    std::vector<AABB> m_aabbs;

    GameState m_gameState = GameState::INITIALIZED;
  };
}

