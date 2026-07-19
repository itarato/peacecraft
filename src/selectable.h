#pragma once

#include "raylib.h"

struct Selectable {
  Selectable() = default;
  virtual ~Selectable() = default;

  void select();
  void deselect();
  [[nodiscard]] bool is_selected() const;
  void selectable_update(Camera2D& camera);
  [[nodiscard]] virtual bool check_selection_collision(Vector2 selection_pos);
  [[nodiscard]] virtual bool is_selectable() const;

 protected:
  bool selected{};
};
