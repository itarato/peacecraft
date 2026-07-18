#pragma once

#include "common.h"
#include "positionable.h"
#include "raylib.h"

constexpr int RESOURCE_COUNT = 2;
inline const char* RESOURCE_NAMES[RESOURCE_COUNT] = {"Mineral", "Wood"};
constexpr int RESOURCE_MINERAL = 0;
constexpr int RESOURCE_WOOD = 1;
constexpr float RESOURCE_RADIUS{24.f};

inline unsigned int resource_id_provider{0};

struct Resource : Positionable {
  unsigned int id;
  int kind;

  Resource(const int kind, const Vector2 pos) : Positionable(pos), kind(kind) {
    id = resource_id_provider++;

    switch (kind) {
      case RESOURCE_WOOD:
        value = 100;
        break;
      case RESOURCE_MINERAL:
        value = 1000;
        break;
      default:
        bail("Unexpected");
    }
  }

  void draw() const;
  [[nodiscard]] bool is_removable() const;
  [[nodiscard]] Rectangle frame() const;
  int harvest(int request);

 private:
  int value;
};
