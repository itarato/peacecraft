#pragma once

#include "raylib.h"

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
    if (is_selectable() && IsMouseButtonPressed(0) &&
        check_selection_collision(GetScreenToWorld2D(GetMousePosition(), camera))) {
      select();
    }
  }

  virtual bool check_selection_collision(Vector2 selection_pos) {
    return false;
  }

  virtual bool is_selectable() const {
    return true;
  }

 protected:
  bool selected{};
};
