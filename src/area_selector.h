#pragma once

#include "raylib.h"
#include "raymath.h"

struct AreaSelector {
  AreaSelector() {
  }

  void update(Camera2D& camera);
  void draw() const;
  bool just_selected() const;
  Rectangle selection_frame() const;

 private:
  Vector2 selection_start{};
  Vector2 selection_end{};
  bool is_selection{};
  bool is_just_selected{};
};
