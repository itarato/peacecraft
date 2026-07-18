#include "resource.h"

#include "common.h"
#include "positionable.h"
#include "raylib.h"

void Resource::draw() const {
  Color color;
  if (kind == RESOURCE_WOOD) {
    color = BROWN;
  } else if (kind == RESOURCE_MINERAL) {
    color = MAGENTA;
  } else {
    bail("Unexpected");
  }

  DrawCircleV(pos, RESOURCE_RADIUS, color);
}

[[nodiscard]] bool Resource::is_removable() const {
  return value <= 0;
}

[[nodiscard]] Rectangle Resource::frame() const {
  return {pos.x - RESOURCE_RADIUS, pos.y - RESOURCE_RADIUS, RESOURCE_RADIUS * 2.f, RESOURCE_RADIUS * 2.f};
}

int Resource::harvest(int request) {
  const int actual = std::min(request, value);
  value -= actual;
  return actual;
}
