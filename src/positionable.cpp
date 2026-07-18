#include "positionable.h"

#include "common.h"
#include "raylib.h"

Vector2Int Positionable::grid_pos() const {
  return vector2_to_grid_pos(pos);
}
