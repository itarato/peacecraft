#include "movable.h"

#include "positionable.h"
#include "raylib.h"

void Movable::set_move_target(Vector2 new_move_target) {
  move_target = new_move_target;
}
