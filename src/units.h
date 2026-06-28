#pragma once

#include "common.h"
#include "raylib.h"
#include "selection.h"

#define UNIT_WIDTH 40
#define UNIT_HEIGHT 60

struct Positionable {
  Positionable() {
  }

  Positionable(Vector2 pos) : pos(pos) {
  }

  Vector2Int grid_pos() const {
    return vector2_to_grid_pos(pos);
  }

 protected:
  Vector2 pos{};
};

struct Selectable {
  Selectable() {
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

 protected:
  bool selected{};
};

struct Movable : Positionable {
  Movable(Vector2 move_target) : Positionable(move_target), move_target(move_target) {
  }

  void set_move_target(Vector2 new_move_target) {
    move_target = new_move_target;
  }

 protected:
  Vector2 move_target{};
  float speed{30.0};
};

struct Character : Movable, Selectable {
  Character(Vector2 pos) : Movable(pos) {
  }

  void draw() const {
    DrawRectangleRec(frame(), DARKBROWN);

    if (selected) {
      DrawRectangleLinesEx(frame(), 2, ORANGE);
    }

    DrawCircleV(grid_pos_to_vector2(grid_pos()), 4, RED);
    DrawCircleV(move_target, 4, PURPLE);
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

  Rectangle frame() const {
    return Rectangle{pos.x - (UNIT_WIDTH >> 1), pos.y - (UNIT_HEIGHT >> 1), UNIT_WIDTH, UNIT_HEIGHT};
  }
};

struct Building : Positionable {
  Building(Vector2 pos) : Positionable(pos) {
  }

  void draw() const {
  }

  void update() {
  }
};
