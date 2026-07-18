#pragma once

#include "raylib.h"

struct Selectable {
  Selectable() {
  }
  virtual ~Selectable() = default;

  void select();
  void deselect();
  bool is_selected() const;
  void selectable_update(Camera2D& camera);
  virtual bool check_selection_collision(Vector2 selection_pos);
  virtual bool is_selectable() const;

 protected:
  bool selected{};
};
