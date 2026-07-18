#include "characters.h"

#include <limits>
#include <vector>

#include "buildings.h"
#include "commands.h"
#include "common.h"
#include "movable.h"
#include "raylib.h"
#include "raymath.h"
#include "resource.h"
#include "selectable.h"

void Character::draw() const {
  DrawRectangleRec(frame(), CHARACTER_KIND_COLOR[group]);

  if (selected) {
    DrawRectangleLinesEx(frame(), 2, ORANGE);
  }

  DrawCircleV(grid_pos_to_vector2(grid_pos()), 4, RED);
  DrawCircleV(move_target, 4, PURPLE);

  Rectangle health_bar_frame = frame();
  health_bar_frame.height = 8;
  health_bar_frame.y -= 10;
  DrawRectangleRec(health_bar_frame, BLACK);
  health_bar_frame.width *= health / CHARACTER_MAX_HEALTH;
  DrawRectangleRec(health_bar_frame, RED);
}

void Character::update(Camera2D& camera, std::unordered_map<unsigned int, Character>& characters,
                       std::vector<Building>& buildings) {
  selectable_update(camera);

  float target_distance = Vector2Distance(pos, move_target);
  if (target_distance != 0.0) {
    float travel_distance = GetFrameTime() * get_speed();

    if (travel_distance >= target_distance) {
      pos = move_target;
    } else {
      pos = Vector2Lerp(pos, move_target, travel_distance / target_distance);
    }
  }

  update_attack(characters);
  update_building(buildings);
}

[[nodiscard]] Rectangle Character::frame() const {
  return Rectangle{pos.x - (CHARACTER_WIDTH >> 1), pos.y - (CHARACTER_HEIGHT >> 1), CHARACTER_WIDTH, CHARACTER_HEIGHT};
}

bool Character::check_selection_collision(const Vector2 selection_pos) {
  return CheckCollisionPointRec(selection_pos, frame());
}

[[nodiscard]] bool Character::is_selectable() const {
  return group == PLAYER_CHARACTER_GROUP;
}

void Character::suffer_damage(const float damage) {
  health -= damage;
  if (health < 0.0) health = 0.0;
}

[[nodiscard]] bool Character::is_removable() const {
  return health <= 0.0;
}

[[nodiscard]] CommandList Character::commands() const {
  return CommandList({BuildingCreationRequestCommand{id}});
}

void Character::receive_resource(const int amount, const int kind) {
  resource_amounts[kind] += amount;
}

void Character::empty_resources() {
  for (int& resource_amount : resource_amounts) resource_amount = 0;
}

[[nodiscard]] int Character::resource_amount(const int kind) const {
  return resource_amounts[kind];
}

void Character::update_attack(std::unordered_map<unsigned int, Character>& characters) {
  attack_countdown.update();

  for (auto& [_id, other_character] : characters) {
    if (other_character.group == group) continue;
    if (Vector2Distance(pos, other_character.pos) >= ATTACK_DISTANCE) continue;

    if (attack_countdown.is_finished()) {
      other_character.suffer_damage(attack_power());
      attack_countdown.reset();
    }
  }
}

void Character::update_building(std::vector<Building>& buildings) {
  building_countdown.update();

  for (auto& b : buildings) {
    if (b.is_complete() || Vector2Distance(pos, b.pos) >= BUILDING_DISTANCE) continue;
    if (!building_countdown.is_finished()) continue;

    b.build();
    building_countdown.reset();
  }
}

[[nodiscard]] float Character::attack_power() const {
  if (group == PLAYER_CHARACTER_GROUP) {
    return 10.0;
  }

  if (group == ENEMY_CHARACTER_GROUP) {
    return 8.0;
  }

  bail("Unexpected");
}

float Character::get_speed() const {
  return 50.0f;
}

Vector2 Character::find_closest_building_pos(const std::vector<Building>& buildings) const {
  Vector2 building_pos{};
  float closest = std::numeric_limits<float>::max();

  for (const auto& b : buildings) {
    float dist = Vector2Distance(pos, b.pos);
    if (dist < closest) {
      closest = dist;
      building_pos = b.pos;
    }
  }

  return building_pos;
}
