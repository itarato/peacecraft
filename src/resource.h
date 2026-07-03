#pragma once

#include "common.h"
#include "positionable.h"
#include "raylib.h"

enum class ResourceKind {
  Mineral,
  Wood,
};

struct Resource : Positionable {
  Resource(ResourceKind kind, Vector2 pos) : Positionable(pos), kind(kind) {
  }

  void draw(const Camera2D& camera) const {
  }

 private:
  ResourceKind kind;
};
