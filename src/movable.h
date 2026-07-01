#pragma once

#include "positionable.h"
#include "raylib.h"

struct Movable : Positionable {
  Movable(Vector2 move_target) : Positionable(move_target), move_target(move_target) {
  }

  void set_move_target(Vector2 new_move_target) {
    move_target = new_move_target;
  }

 protected:
  Vector2 move_target{};
};