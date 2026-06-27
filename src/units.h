#pragma once

#include "raylib.h"
#include "selection.h"

#define UNIT_WIDTH 40
#define UNIT_HEIGHT 60

struct Unit {
  Unit(Vector2 pos) : pos(pos) {
  }

  void update() {
  }

  void draw() const {
    DrawRectangleRec(frame(), DARKGREEN);
  }

 private:
  Vector2 pos;

  Rectangle frame() const {
    return Rectangle{pos.x - (UNIT_WIDTH >> 1), pos.y - (UNIT_HEIGHT >> 1), UNIT_WIDTH, UNIT_HEIGHT};
  }
};
