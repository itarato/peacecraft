#pragma once

#include <cmath>
#include <optional>
#include <set>
#include <unordered_set>

#include "raylib.h"

#define MOVE_GRID_SIZE 35

const int GRID_NEIGHBOUR_MAP[8][2] = {
    {-1, -1}, {0, -1}, {1, -1}, {-1, 0}, {1, 0}, {-1, 1}, {0, 1}, {1, 1},
};

struct Vector2Int {
  int x{};
  int y{};

  Vector2Int() {
  }

  Vector2Int(int x, int y) : x(x), y(y) {
  }

  Vector2Int(Vector2 v) : x(v.x), y(v.y) {
  }

  bool operator==(const Vector2Int& other) const {
    return x == other.x && y == other.y;
  }
};

float vec2int_distance(Vector2Int a, Vector2Int b) {
  float sqx = std::powf((float)(a.x - b.x), 2.f);
  float sqy = std::powf((float)(a.y - b.y), 2.f);
  return std::sqrtf(sqx + sqy);
}

namespace std {
template <>
struct hash<Vector2Int> {
  size_t operator()(const Vector2Int& v) const {
    size_t h1 = std::hash<int>{}(v.x);
    size_t h2 = std::hash<int>{}(v.y);

    return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
  }
};
}  // namespace std

Vector2Int vector2_to_grid_pos(Vector2 v) {
  return Vector2Int((v.x + (MOVE_GRID_SIZE >> 1)) / MOVE_GRID_SIZE, (v.y + (MOVE_GRID_SIZE >> 1)) / MOVE_GRID_SIZE);
}

Vector2 grid_pos_to_vector2(Vector2Int v) {
  return Vector2(v.x * MOVE_GRID_SIZE, v.y * MOVE_GRID_SIZE);
}

struct CharacterCreationCommand {};

struct BuildingCommands {
  std::optional<CharacterCreationCommand> character_creation_command;

  void draw() const {
    int offset = 0;

    if (character_creation_command.has_value()) {
    }
  }
};
