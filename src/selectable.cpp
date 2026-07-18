#include "selectable.h"

#include "raylib.h"

void Selectable::select() {
  selected = true;
}

void Selectable::deselect() {
  selected = false;
}

bool Selectable::is_selected() const {
  return selected;
}

void Selectable::selectable_update(Camera2D& camera) {
  if (is_selectable() && IsMouseButtonPressed(0) &&
      check_selection_collision(GetScreenToWorld2D(GetMousePosition(), camera))) {
    select();
  }
}

bool Selectable::check_selection_collision(Vector2 selection_pos) {
  return false;
}

bool Selectable::is_selectable() const {
  return true;
}
