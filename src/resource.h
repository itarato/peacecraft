#pragma once

#include "common.h"
#include "positionable.h"
#include "raylib.h"

static unsigned int resource_id_provider{0};
constexpr float RESOURCE_RADIUS{24.f};

enum class ResourceKind {
  Mineral,
  Wood,
};

struct Resource : Positionable {
  unsigned int id;

  Resource(const ResourceKind kind, const Vector2 pos) : Positionable(pos), kind(kind) {
    id = resource_id_provider++;

    switch (kind) {
      case ResourceKind::Wood:
        value = 100;
        break;
      case ResourceKind::Mineral:
        value = 1000;
        break;
      default:
        bail("Unexpected");
    }
  }

  void draw(const Camera2D& camera) const {
    Color color;
    if (kind == ResourceKind::Wood) {
      color = BROWN;
    } else if (kind == ResourceKind::Mineral) {
      color = MAGENTA;
    } else {
      bail("Unexpected");
    }

    DrawCircleV(GetScreenToWorld2D(pos, camera), RESOURCE_RADIUS, color);
  }

  [[nodiscard]] bool is_removable() const {
    return value <= 0;
  }

  [[nodiscard]] Rectangle frame() const {
    return {pos.x - RESOURCE_RADIUS, pos.y - RESOURCE_RADIUS, RESOURCE_RADIUS * 2.f, RESOURCE_RADIUS * 2.f};
  }

  int harvest(int request) {
    const int actual = std::min(request, value);
    value -= actual;
    return actual;
  }

 private:
  ResourceKind kind;
  int value;
};
