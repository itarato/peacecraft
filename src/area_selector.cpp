#include "area_selector.h"

#include "raylib.h"
#include "raymath.h"

void AreaSelector::update(Camera2D& camera) {
  is_just_selected = false;

  if (IsMouseButtonPressed(0)) {
    is_selection = true;
    selection_start = GetScreenToWorld2D(GetMousePosition(), camera);
  }

  if (is_selection) {
    selection_end = GetScreenToWorld2D(GetMousePosition(), camera);

    if (IsMouseButtonReleased(0)) {
      is_selection = false;
      is_just_selected = selection_start != selection_end;
    }
  }
}

void AreaSelector::draw() const {
  if (is_selection) {
    DrawRectangleLinesEx(selection_frame(), 2.f, BLUE);
    DrawRectangleRec(selection_frame(), Fade(BLUE, 0.1));
  }
}

bool AreaSelector::just_selected() const {
  return is_just_selected;
}

Rectangle AreaSelector::selection_frame() const {
  Vector2 min = Vector2Min(selection_start, selection_end);
  Vector2 max = Vector2Max(selection_start, selection_end);
  Vector2 diff = Vector2Subtract(max, min);
  return Rectangle(min.x, min.y, diff.x, diff.y);
}
