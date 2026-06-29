#pragma once

#include "common.h"
#include "raylib.h"
#include "selection.h"

#define CHARACTER_WIDTH 40
#define CHARACTER_HEIGHT 60

#define BUILDING_SIZE 80

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

  void selectable_update(Camera2D& camera) {
    if (IsMouseButtonPressed(0) && check_selection_collision(GetScreenToWorld2D(GetMousePosition(), camera))) {
      select();
    }
  }

  virtual bool check_selection_collision(Vector2 selection_pos) {
    return false;
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
};

struct Building : Positionable, Selectable {
  BuildingCommands buildind_commands;

  Building(Vector2 pos) : Positionable(pos) {
    buildind_commands = BuildingCommands{CharacterCreationCommand{pos}};
  }

  void draw() const {
    DrawCircleV(pos, BUILDING_SIZE >> 1, BLACK);

    if (selected) {
      DrawCircleLinesEx(pos, BUILDING_SIZE >> 1, 2, ORANGE);
    }
  }

  void update(Camera2D& camera) {
    selectable_update(camera);
  }

  bool check_selection_collision(Vector2 selection_pos) override {
    return CheckCollisionPointCircle(selection_pos, pos, BUILDING_SIZE >> 1);
  }
};
