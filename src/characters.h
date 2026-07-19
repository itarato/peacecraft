#pragma once

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

constexpr int CHARACTER_WIDTH = 40;
constexpr int CHARACTER_HEIGHT = 60;

constexpr int PLAYER_CHARACTER_GROUP = 0;
constexpr int ENEMY_CHARACTER_GROUP = 1;

constexpr float ATTACK_DISTANCE = 60.0f;
constexpr float BUILDING_DISTANCE = 60.0f;
constexpr float CHARACTER_MAX_HEALTH = 100.0f;

constexpr Color CHARACTER_KIND_COLOR[2] = {BROWN, VIOLET};

inline unsigned int character_id_provider{0};

struct Character : Movable, Selectable {
  unsigned int id;
  int group;

  Character(const Character&) = delete;
  Character& operator=(const Character&) = delete;

  Character(Character&&) = default;
  Character& operator=(Character&&) = default;

  Character(Vector2 pos, int group) : Movable(pos), group(group) {
    under_attack_indicator_countdown.finish();
    id = character_id_provider++;
  }

  void draw() const;
  void update(Camera2D& camera, std::unordered_map<unsigned int, Character>& characters,
              std::unordered_map<unsigned int, Building>& buildings);
  [[nodiscard]] Rectangle frame() const;
  bool check_selection_collision(const Vector2 selection_pos) override;
  [[nodiscard]] bool is_selectable() const override;
  void suffer_damage(const float damage);
  [[nodiscard]] bool is_removable() const;
  [[nodiscard]] CommandList commands() const;
  void receive_resource(const int amount, const int kind);
  void empty_resources();
  [[nodiscard]] int resource_amount(const int kind) const;
  [[nodiscard]] bool is_under_attack() const;

 private:
  float health{CHARACTER_MAX_HEALTH};
  Countdown attack_countdown{0.5f};
  Countdown building_countdown{0.1f};
  int resource_amounts[RESOURCE_COUNT] = {};
  Countdown under_attack_indicator_countdown{2.f};

  void update_attack(std::unordered_map<unsigned int, Character>& characters);
  void update_building(std::unordered_map<unsigned int, Building>& buildings);
  [[nodiscard]] float attack_power() const;
  float get_speed() const;
  Vector2 find_closest_building_pos(const std::unordered_map<unsigned int, Building>& buildings) const;
};
