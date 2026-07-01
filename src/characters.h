#pragma once

#include <vector>

#include "common.h"
#include "movable.h"
#include "raylib.h"
#include "raymath.h"
#include "selectable.h"

#define CHARACTER_WIDTH 40
#define CHARACTER_HEIGHT 60

#define PLAYER_CHARACTER_GROUP 0
#define ENEMY_CHARACTER_GROUP 1

#define ATTACK_DISTANCE 50.0f
#define CHARACTER_MAX_HEALTH 100.0f

const Color CHARACTER_KIND_COLOR[2] = {BROWN, VIOLET};

static unsigned int character_id_provider{0};

struct Character : Movable, Selectable {
  Character(const Character&) = delete;
  Character& operator=(const Character&) = delete;

  Character(Character&&) = default;
  Character& operator=(Character&&) = default;

  Character(Vector2 pos, int group) : Movable(pos), group(group) {
    id = character_id_provider++;
  }

  void draw() const {
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

  void update(Camera2D& camera, std::vector<Character>& all_characters) {
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

    update_attack(all_characters);
  }

  Rectangle frame() const {
    return Rectangle{pos.x - (CHARACTER_WIDTH >> 1), pos.y - (CHARACTER_HEIGHT >> 1), CHARACTER_WIDTH,
                     CHARACTER_HEIGHT};
  }

  bool check_selection_collision(Vector2 selection_pos) override {
    return CheckCollisionPointRec(selection_pos, frame());
  }

  bool is_selectable() const override {
    return group == PLAYER_CHARACTER_GROUP;
  }

  void suffer_damage(float damage) {
    TraceLog(LOG_INFO, "DAMAGE #%lu", id);
    health -= damage;
    if (health < 0.0) health = 0.0;
  }

  bool is_removable() const {
    if (health <= 0.0) TraceLog(LOG_INFO, "DYING");

    return health <= 0.0;
  }

 private:
  unsigned int id;
  int group;
  float health{CHARACTER_MAX_HEALTH};
  Countdown attack_countdown{0.5f};

  void update_attack(std::vector<Character>& all_characters) {
    attack_countdown.update();

    for (auto& other_character : all_characters) {
      if (other_character.group == group) continue;
      if (Vector2Distance(pos, other_character.pos) >= ATTACK_DISTANCE) continue;

      if (attack_countdown.is_finished()) {
        other_character.suffer_damage(attack_power());
        attack_countdown.reset();
      }
    }
  }

  float attack_power() const {
    if (group == PLAYER_CHARACTER_GROUP) {
      return 10.0;
    } else if (group == ENEMY_CHARACTER_GROUP) {
      return 8.0;
    } else {
      return UNEXPECTED;
    }
  }

  float get_speed() const {
    return 50.0f;
  }
};
