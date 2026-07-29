#include "universal_entity.h"

#include <memory>
#include <vector>

#include "automations.h"

std::vector<std::shared_ptr<Automation>> BuildingMarkerUEntity::update(Camera2D& camera) {
  if (IsMouseButtonPressed(0)) {
    removable = true;
    Vector2 target = GetScreenToWorld2D(GetMousePosition(), camera);
    return {std::make_shared<BuildingAutomation>(1.f, target, PLAYER_CHARACTER_GROUP),
            std::make_shared<MoveAutomation>(1.f, character_id, target)};
  }

  return {};
}

void BuildingMarkerUEntity::draw(const Camera2D& camera) const {
  DrawCircleLinesEx(GetScreenToWorld2D(GetMousePosition(), camera), 32.0f, 4, LIME);
}

bool BuildingMarkerUEntity::is_removable() const {
  return removable;
}
