#pragma once

#include "common.h"
#include "movable.h"
#include "raylib.h"
#include "raymath.h"
#include "selectable.h"

#define CHARACTER_WIDTH 40
#define CHARACTER_HEIGHT 60

#define PLAYER_CHARACTER_KIND 0
#define ENEMY_CHARACTER_KIND 1

const Color CHARACTER_KIND_COLOR[2] = {BROWN, VIOLET};

struct Character : Movable, Selectable {
  Character(Vector2 pos, int kind) : Movable(pos), kind(kind) {
  }

  void draw() const {
    DrawRectangleRec(frame(), CHARACTER_KIND_COLOR[kind]);

    if (selected) {
      DrawRectangleLinesEx(frame(), 2, ORANGE);
    }

    DrawCircleV(grid_pos_to_vector2(grid_pos()), 4, RED);
    DrawCircleV(move_target, 4, PURPLE);
  }

  void update(Camera2D& camera) {
    selectable_update(camera);

    float target_distance = Vector2Distance(pos, move_target);
    if (target_distance != 0.0) {
      float travel_distance = GetFrameTime() * speed;

      if (travel_distance >= target_distance) {
        pos = move_target;
      } else {
        pos = Vector2Lerp(pos, move_target, travel_distance / target_distance);
      }
    }
  }

  Rectangle frame() const {
    return Rectangle{pos.x - (CHARACTER_WIDTH >> 1), pos.y - (CHARACTER_HEIGHT >> 1), CHARACTER_WIDTH,
                     CHARACTER_HEIGHT};
  }

  bool check_selection_collision(Vector2 selection_pos) override {
    return CheckCollisionPointRec(selection_pos, frame());
  }

  bool is_selectable() const override {
    return kind == PLAYER_CHARACTER_KIND;
  }

 private:
  int kind;
};
