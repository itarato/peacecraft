#pragma once

#include "raylib.h"
#include "raymath.h"

struct Selector {
  Selector() {
  }

  void update() {
    if (IsMouseButtonPressed(0)) {
      is_selection = true;
      selection_start = GetMousePosition();
    }

    if (is_selection) {
      selection_end = GetMousePosition();

      if (IsMouseButtonReleased(0)) {
        is_selection = false;
      }
    }
  }

  void draw() const {
    if (is_selection) {
      DrawRectangleLinesEx(frame(), 2.f, BLUE);
      DrawRectangleRec(frame(), Fade(BLUE, 0.1));
    }
  }

 private:
  Vector2 selection_start{};
  Vector2 selection_end{};
  bool is_selection{};

  Rectangle frame() const {
    Vector2 min = Vector2Min(selection_start, selection_end);
    Vector2 max = Vector2Max(selection_start, selection_end);
    Vector2 diff = Vector2Subtract(max, min);
    return Rectangle(min.x, min.y, diff.x, diff.y);
  }
};
