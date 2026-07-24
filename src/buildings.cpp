#include "buildings.h"

#include "characters.h"
#include "commands.h"
#include "common.h"
#include "positionable.h"
#include "raylib.h"
#include "selectable.h"

void Building::draw() const {
  if (!is_complete()) {
    DrawCircleV(pos, BUILDING_SIZE >> 1, Fade(DARKBLUE, completeness));
    DrawCircleLinesEx(pos, BUILDING_SIZE >> 1, 2.0f, BLACK);
  } else {
    DrawCircleV(pos, BUILDING_SIZE >> 1, BLACK);
  }

  if (selected) {
    DrawCircleLinesEx(pos, BUILDING_SIZE >> 1, 2, ORANGE);
  }
}

void Building::update(Camera2D& camera) {
  selectable_update(camera);
}

bool Building::check_selection_collision(Vector2 selection_pos) {
  return CheckCollisionPointCircle(selection_pos, pos, BUILDING_SIZE >> 1);
}

CommandList Building::commands() const {
  return CommandList({CharacterCreationCommand{id}});
}

bool Building::is_selectable() const {
  return group == PLAYER_CHARACTER_GROUP && completeness == 1.0f;
}

bool Building::is_complete() const {
  return completeness == 1.0f;
}

void Building::build() {
  completeness += build_increase;
  if (completeness > 1.0f) completeness = 1.0f;
}

std::vector<Vector2Int> Building::covered_grid(float padding) const {
  std::vector<Vector2Int> out{};

  int minx = pos_to_grid(pos.x - (BUILDING_SIZE >> 1) - padding);
  int maxx = pos_to_grid(pos.x + (BUILDING_SIZE >> 1) + padding);
  int miny = pos_to_grid(pos.y - (BUILDING_SIZE >> 1) - padding);
  int maxy = pos_to_grid(pos.y + (BUILDING_SIZE >> 1) + padding);

  for (int y = miny; y <= maxy; y++) {
    for (int x = minx; x <= maxx; x++) {
      out.emplace_back(x, y);
    }
  }

  return out;
}
