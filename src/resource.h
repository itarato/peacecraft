#pragma once

#include "common.h"
#include "positionable.h"
#include "raylib.h"

constexpr float RESOURCE_RADIUS{24.f};

enum class ResourceKind {
  Mineral,
  Wood,
};

struct Resource : Positionable {
  Resource(ResourceKind kind, Vector2 pos) : Positionable(pos), kind(kind) {
    switch (kind) {
      case ResourceKind::Wood:
        value = 100.0f;
        break;
      case ResourceKind::Mineral:
        value = 1000.0f;
        break;
      default:
        UNEXPECTED;
    }
  }

  void draw(const Camera2D& camera) const {
    Color color;
    if (kind == ResourceKind::Wood) {
      color = BROWN;
    } else if (kind == ResourceKind::Mineral) {
      color = MAGENTA;
    } else {
      UNEXPECTED;
    }

    DrawCircleV(GetScreenToWorld2D(pos, camera), RESOURCE_RADIUS, color);
  }

  bool is_removable() const {
    return value <= 0.0f;
  }

  Rectangle frame() const {
    return {pos.x - RESOURCE_RADIUS, pos.y - RESOURCE_RADIUS, RESOURCE_RADIUS * 2.f, RESOURCE_RADIUS * 2.f};
  }

 private:
  ResourceKind kind;
  float value;
};
