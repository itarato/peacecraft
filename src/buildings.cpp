#include "buildings.h"

#include "characters.h"
#include "commands.h"
#include "common.h"
#include "positionable.h"
#include "raylib.h"
#include "selectable.h"

void Building::draw() const {
  DrawCircleV(pos, BUILDING_SIZE >> 1, Fade(BLACK, completeness));

  if (!is_complete()) {
    DrawCircleLinesEx(pos, BUILDING_SIZE >> 1, 2.0f, BLACK);
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
  return CommandList({CharacterCreationCommand{pos}});
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
