#include "grid_explorer.h"

#include "common.h"

// Required for std::set<>.
bool GridExploreNode::operator<(const GridExploreNode& other) const {
  if (score == other.score) {
    if (pos.x == other.pos.x) {
      return pos.y < other.pos.y;
    }
    return pos.x < other.pos.x;
  }

  return score < other.score;
}

Vector2Int GridPosExplorer::next_available() {
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
