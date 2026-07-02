#pragma once

#include "common.h"
#include "raylib.h"

struct Positionable {
  Vector2 pos{};

  Positionable() {
  }

  Positionable(Vector2 pos) : pos(pos) {
  }

  Vector2Int grid_pos() const {
    return vector2_to_grid_pos(pos);
  }
};