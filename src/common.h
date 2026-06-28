#pragma once

#include <cmath>
#include <optional>
#include <set>
#include <unordered_set>

#include "raylib.h"

#define MOVE_GRID_SIZE 35

#define COMMAND_ICON_SIZE 60
#define COMMAND_ICON_PADDING 10

struct Vector2Int {
  int x{};
  int y{};

  Vector2Int() {
  }

  Vector2Int(int x, int y) : x(x), y(y) {
  }

  Vector2Int(Vector2 v) : x(v.x), y(v.y) {
  }

  bool operator==(const Vector2Int& other) const {
    return x == other.x && y == other.y;
  }
};

float vec2int_distance(Vector2Int a, Vector2Int b) {
  float sqx = std::powf((float)(a.x - b.x), 2.f);
  float sqy = std::powf((float)(a.y - b.y), 2.f);
  return std::sqrtf(sqx + sqy);
}

namespace std {
template <>
struct hash<Vector2Int> {
  size_t operator()(const Vector2Int& v) const {
    size_t h1 = std::hash<int>{}(v.x);
    size_t h2 = std::hash<int>{}(v.y);

    return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
  }
};
}  // namespace std

Vector2Int vector2_to_grid_pos(Vector2 v) {
  return Vector2Int((v.x + (MOVE_GRID_SIZE >> 1)) / MOVE_GRID_SIZE, (v.y + (MOVE_GRID_SIZE >> 1)) / MOVE_GRID_SIZE);
}

Vector2 grid_pos_to_vector2(Vector2Int v) {
  return Vector2(v.x * MOVE_GRID_SIZE, v.y * MOVE_GRID_SIZE);
}

enum class GameCommandType {
  CharacterCreation,
};

struct CharacterCreationCommand {
  Vector2 base_pos;
};

struct GameCommand {
  GameCommandType type;
  union {
    CharacterCreationCommand character_creation_command;
  };
};

struct BuildingCommands {
  std::optional<CharacterCreationCommand> character_creation_command;

  void draw() const {
    int index = 0;

    if (character_creation_command.has_value()) {
      Rectangle icon_frame = get_icon_frame(index);
      DrawRectangleRec(icon_frame, GOLD);
      DrawText("Make character", COMMAND_ICON_PADDING, icon_frame.y + COMMAND_ICON_SIZE + 2, 10, BLACK);

      index += 1;
    }
  }

  std::optional<GameCommand> selected_command() {
    int index = 0;

    if (character_creation_command.has_value()) {
      Rectangle icon_frame = get_icon_frame(index);
      if (IsMouseButtonPressed(0) && CheckCollisionPointRec(GetMousePosition(), icon_frame)) {
        return GameCommand{GameCommandType::CharacterCreation, character_creation_command.value()};
      }

      index += 1;
    }

    return std::nullopt;
  }

 private:
  Rectangle get_icon_frame(int index) const {
    return Rectangle(COMMAND_ICON_PADDING, COMMAND_ICON_PADDING + (COMMAND_ICON_PADDING + COMMAND_ICON_SIZE) * index,
                     COMMAND_ICON_SIZE, COMMAND_ICON_SIZE);
  }
};
