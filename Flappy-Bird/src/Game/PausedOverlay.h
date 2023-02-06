#pragma once
#include "Core/Layer.h"
#include "Entity/Entity.h"
#include "Colliders/AABB.h"
#include "Colliders/CircleCollider.h"
#include "Game/Obstacle.h"

namespace WallpaperAPI
{
  class PausedOverlay : public Layer
  {
  public:
    PausedOverlay();

    void OnAttach() override;
    void OnDetach() override;

    void OnUpdate(float delta) override;
    void OnImGuiRender() override;

    const std::string& GetID() const override { return ID; }
    static const std::string& ID;
  };
}

