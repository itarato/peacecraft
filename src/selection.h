#pragma once

#include "raylib.h"
#include "raymath.h"

struct AreaSelector {
  AreaSelector() {
  }

  void update() {
    is_just_selected = false;

    if (IsMouseButtonPressed(0)) {
      is_selection = true;
      selection_start = GetMousePosition();
    }

    if (is_selection) {
      selection_end = GetMousePosition();

      if (IsMouseButtonReleased(0)) {
        is_selection = false;
        is_just_selected = selection_start != selection_end;
      }
    }
  }

  void draw() const {
    if (is_selection) {
      DrawRectangleLinesEx(selection_frame(), 2.f, BLUE);
      DrawRectangleRec(selection_frame(), Fade(BLUE, 0.1));
    }
  }

  bool just_selected() const {
    return is_just_selected;
  }

  Rectangle selection_frame() const {
    Vector2 min = Vector2Min(selection_start, selection_end);
    Vector2 max = Vector2Max(selection_start, selection_end);
    Vector2 diff = Vector2Subtract(max, min);
    return Rectangle(min.x, min.y, diff.x, diff.y);
  }

 private:
  Vector2 selection_start{};
  Vector2 selection_end{};
  bool is_selection{};
  bool is_just_selected{};
};
