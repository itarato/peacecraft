#pragma once

#include "common.h"
#include "raylib.h"
#include "selection.h"

#define UNIT_WIDTH 40
#define UNIT_HEIGHT 60

struct Unit {
  Unit(Vector2 pos) : pos(pos), move_target(pos) {
  }

  void update() {
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

  void draw() const {
    DrawRectangleRec(frame(), DARKBROWN);

    if (selected) {
      DrawRectangleLinesEx(frame(), 2, ORANGE);
    }

    DrawCircleV(grid_pos_to_vector2(grid_pos()), 4, RED);
    DrawCircleV(move_target, 4, PURPLE);
  }

  Rectangle frame() const {
    return Rectangle{pos.x - (UNIT_WIDTH >> 1), pos.y - (UNIT_HEIGHT >> 1), UNIT_WIDTH, UNIT_HEIGHT};
  }

  void select() {
    selected = true;
  }

  void deselect() {
    selected = false;
  }

  bool is_selected() const {
    return selected;
  }

  void set_move_target(Vector2 new_move_target) {
    move_target = new_move_target;
  }

  Vector2Int grid_pos() const {
    return vector2_to_grid_pos(pos);
  }

 private:
  Vector2 pos{};
  bool selected{};
  Vector2 move_target{};
  float speed{30.0};
};
