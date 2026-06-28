#pragma once

#include "common.h"

const int GRID_NEIGHBOUR_MAP[8][2] = {
    {-1, -1}, {0, -1}, {1, -1}, {-1, 0}, {1, 0}, {-1, 1}, {0, 1}, {1, 1},
};

struct GridExploreNode {
  // Position to explore.
  Vector2Int pos;
  float score;

  GridExploreNode(Vector2Int pos, Vector2Int current_pos, Vector2Int target_pos) : pos(pos) {
    // Score is calculated by the distance between the target and explored position (pos) minus
    // the square rooted distance between the current position and the explored position (pos).
    // This is so we favour same target distance nodes when it's closer to the target -
    // so they don't need to go "over the target" - but not too much, hence the square root.
    float pos_end_diff = vec2int_distance(target_pos, pos);
    float pos_start_diff = vec2int_distance(current_pos, pos);
    score = pos_end_diff + std::sqrtf(pos_start_diff);
  }

  bool operator<(const GridExploreNode& other) const {
    if (score == other.score) {
      if (pos.x == other.pos.x) {
        return pos.y < other.pos.y;
      }
      return pos.x < other.pos.x;
    }

    return score < other.score;
  }
};

struct GridPosExplorer {
  GridPosExplorer(Vector2Int current_pos, Vector2Int target_pos, std::unordered_set<Vector2Int> occupied)
      : current_pos(current_pos), target_pos(target_pos), occupied(occupied) {
    queue.insert(GridExploreNode(target_pos, current_pos, target_pos));
    visited.insert(target_pos);
  }

  Vector2Int next_available() {
    while (true) {
      auto node_it = queue.extract(queue.begin());
      GridExploreNode node(node_it.value());

      // If available - return.
      if (!occupied.contains(node.pos)) {
        return node.pos;
      }

      for (const auto& [offs_x, offs_y] : GRID_NEIGHBOUR_MAP) {
        Vector2Int neighbour_grid = Vector2Int(node.pos.x + offs_x, node.pos.y + offs_y);
        // If not yet visited - enqueue.
        if (!visited.contains(neighbour_grid)) {
          queue.insert(GridExploreNode(neighbour_grid, current_pos, target_pos));
          visited.insert(neighbour_grid);
        }
      }
    }
  }

 private:
  // Grid pos.
  Vector2Int current_pos;
  // Grid pos.
  Vector2Int target_pos;

  // Has a unit already.
  std::unordered_set<Vector2Int> occupied;
  // Explored nodes.
  std::unordered_set<Vector2Int> visited{};

  // Work set.
  std::set<GridExploreNode> queue{};
};
