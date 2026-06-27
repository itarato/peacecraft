#pragma once

#include "raylib.h"

#define MOVE_GRID_SIZE 35

struct Vector2Int {
  int x{};
  int y{};

  Vector2Int() {
  }

  Vector2Int(int x, int y) : x(x), y(y) {
  }

  Vector2Int(Vector2 v) : x(v.x), y(v.y) {
  }
};

Vector2Int vector2_to_grid_pos(Vector2 v) {
  return Vector2Int((v.x + (MOVE_GRID_SIZE >> 1)) / MOVE_GRID_SIZE, (v.y + (MOVE_GRID_SIZE >> 1)) / MOVE_GRID_SIZE);
}

Vector2 grid_pos_to_vector2(Vector2Int v) {
  return Vector2(v.x * MOVE_GRID_SIZE, v.y * MOVE_GRID_SIZE);
}
