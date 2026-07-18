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

  void draw() const;
  void update(Camera2D& camera);
  bool check_selection_collision(Vector2 selection_pos) override;
  CommandList commands() const;
  bool is_selectable() const override;
  bool is_complete() const;
  void build();

 private:
  float completeness{0.0f};
  float build_increase{0.01f};
};
