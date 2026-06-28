#pragma once

#include <cmath>
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

struct GridExploreNode {
  Vector2Int grid_pos;

  GridExploreNode(Vector2Int grid_pos, Vector2Int start_pos, Vector2Int end_pos) : grid_pos(grid_pos) {
    float pos_end_diff = vec2int_distance(end_pos, grid_pos);
    float pos_start_diff = vec2int_distance(start_pos, grid_pos);
    score = pos_end_diff - std::sqrtf(pos_start_diff);
  }

  bool operator<(const GridExploreNode& other) const {
    return score < other.score;
  }

 private:
  float score;
};

struct GridPosExplorer {
  GridPosExplorer(Vector2Int start_pos, Vector2Int end_pos, std::unordered_set<Vector2Int> occupied)
      : start_pos(start_pos), end_pos(end_pos), occupied(occupied) {
    queue.insert(GridExploreNode(end_pos, start_pos, end_pos));
    visited.insert(end_pos);
  }

  Vector2Int next_available() {
    while (true) {
      auto node_it = queue.extract(queue.begin());
      GridExploreNode node(node_it.value());

      if (!occupied.contains(node.grid_pos)) {
        return node.grid_pos;
      }

      for (const auto& [offs_x, offs_y] : GRID_NEIGHBOUR_MAP) {
        Vector2Int neighbour_grid = Vector2Int(node.grid_pos.x + offs_x, node.grid_pos.y + offs_y);
        if (!visited.contains(neighbour_grid)) {
          queue.insert(GridExploreNode(neighbour_grid, start_pos, end_pos));
          visited.insert(neighbour_grid);
        }
      }
    }
  }

 private:
  Vector2Int start_pos;
  Vector2Int end_pos;
  std::unordered_set<Vector2Int> occupied;
  std::unordered_set<Vector2Int> visited{};
  std::set<GridExploreNode> queue{};
};
