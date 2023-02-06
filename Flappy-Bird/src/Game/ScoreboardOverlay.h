#pragma once
#include "Core/Layer.h"
#include "Entity/Entity.h"
#include "Colliders/AABB.h"
#include "Colliders/CircleCollider.h"
#include "Game/Obstacle.h"
#include "Core/Rect.h"

namespace WallpaperAPI
{
  class ScoreboardOverlay : public Layer
  {
  public:
    ScoreboardOverlay();

    void OnAttach() override;
    void OnDetach() override;

    void OnUpdate(float delta) override;
    void OnImGuiRender() override;

    const std::string& GetID() const override { return ID; }
    static const std::string& ID;
  private:
    void CreateScores();
    void Render(float delta);

    void RenderNoScores();
    void RenderSelectedScore();

    Rect<float> &backLocation(Renderer &renderer);

  private:
    struct Score
    {
      size_t score;
      std::string player;
      std::string time;

      float x, y;
    };

  private:
    const float m_heightMax = -0.2f;
    const float m_heightMin = -1.0f;
    std::vector<Score> m_scores;
    std::vector<Entity> m_pipes;
    Texture m_backTexture;
    Rect<float> m_backLocation = { 0 };

    bool m_keyDown = false;
    int m_selected = 0;
  };
}

