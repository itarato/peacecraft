#pragma once

#include "common.h"
#include "positionable.h"
#include "raylib.h"
#include "selectable.h"

#define BUILDING_SIZE 80

struct Building : Positionable, Selectable {
  CommandList commands{};

  Building(const Building&) = delete;
  Building& operator=(const Building&) = delete;

  Building(Building&&) = default;
  Building& operator=(Building&&) = default;

  Building(Vector2 pos) : Positionable(pos) {
    commands = CommandList({CharacterCreationCommand{pos}});
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
