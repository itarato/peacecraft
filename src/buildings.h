#pragma once

#include "commands.h"
#include "common.h"
#include "positionable.h"
#include "raylib.h"
#include "selectable.h"

constexpr int BUILDING_SIZE = 80;

struct Building : Positionable, Selectable {
  int group;

  Building(const Building&) = delete;
  Building& operator=(const Building&) = delete;

  Building(Building&&) = default;
  Building& operator=(Building&&) = default;

  Building(int group, Vector2 pos) : Positionable(pos), group(group) {
  }

  Building(int group, Vector2 pos, float completeness) : Positionable(pos), group(group), completeness(completeness) {
  }

  void draw() const {
    DrawCircleV(pos, BUILDING_SIZE >> 1, Fade(BLACK, completeness));

    if (!is_complete()) {
      DrawCircleLinesEx(pos, BUILDING_SIZE >> 1, 2.0f, BLACK);
    }

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

  CommandList commands() const {
    return CommandList({CharacterCreationCommand{pos}});
  }

  bool is_selectable() const override {
    return completeness == 1.0f;
  }

  bool is_complete() const {
    return completeness == 1.0f;
  }

  void build() {
    completeness += build_increase;
    if (completeness > 1.0f) completeness = 1.0f;
  }

 private:
  float completeness{0.0f};
  float build_increase{0.01f};
};
