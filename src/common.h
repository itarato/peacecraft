#pragma once

#include <cmath>
#include <cstdio>
#include <optional>
#include <set>
#include <unordered_set>
#include <variant>
#include <vector>

#include "raylib.h"

#define MOVE_GRID_SIZE 35

#define COMMAND_ICON_SIZE 60
#define COMMAND_ICON_PADDING 10

#define BAIL(...)                        \
  bail(__FILE__, __LINE__, __VA_ARGS__); \
  std::abort()
#define UNEXPECTED                        \
  bail(__FILE__, __LINE__, "Unexpected"); \
  std::abort()
void bail(const char* fileName, int lineNo, const char* s, ...) {
  va_list args;
  va_start(args, s);

  printf("\x1b[93m%16s\x1b[39m:\x1b[96m%-4d\x1b[0m \x1b[94m", fileName, lineNo);
  vprintf(s, args);
  printf("\x1b[0m\n");

  va_end(args);

  exit(EXIT_FAILURE);
}

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
  BuildingCreation,
};

struct BaseCommand {
  virtual ~BaseCommand() = default;
  virtual const char* get_name() const = 0;
};

struct CharacterCreationCommand : BaseCommand {
  Vector2 base_pos;

  CharacterCreationCommand(Vector2 base_pos) : base_pos(base_pos) {
  }

  const char* get_name() const override {
    return "Make character";
  }
};

struct BuildingCreationCommand : BaseCommand {
  Vector2 target_pos;

  BuildingCreationCommand(Vector2 target_pos) : target_pos(target_pos) {
  }

  const char* get_name() const override {
    return "Make building";
  }
};

typedef std::variant<CharacterCreationCommand, BuildingCreationCommand> CommandVariant;

const char* command_get_name(CommandVariant command) {
  if (std::holds_alternative<CharacterCreationCommand>(command)) {
    return std::get<CharacterCreationCommand>(command).get_name();
  } else if (std::holds_alternative<BuildingCreationCommand>(command)) {
    return std::get<BuildingCreationCommand>(command).get_name();
  } else {
    UNEXPECTED;
  }
}

/**
 * Command list to advertise available commands from a source.
 */
struct CommandList {
  std::vector<CommandVariant> commands{};

  CommandList(std::initializer_list<CommandVariant> commands) : commands(commands) {
  }

  void draw(Camera2D const& camera) const {
    for (unsigned int i = 0; i < commands.size(); i++) {
      Rectangle icon_frame = get_icon_frame(i, camera);
      DrawRectangleRec(icon_frame, GOLD);
      DrawText(command_get_name(commands[i]), icon_frame.x, icon_frame.y + COMMAND_ICON_SIZE + 2, 10, BLACK);
    }
  }

  std::optional<CommandVariant> just_selected_command(Camera2D& camera) {
    for (unsigned int i = 0; i < commands.size(); i++) {
      Rectangle icon_frame = get_icon_frame(i, camera);
      if (IsMouseButtonPressed(0) &&
          CheckCollisionPointRec(GetScreenToWorld2D(GetMousePosition(), camera), icon_frame)) {
        return commands[i];
      }
    }

    return std::nullopt;
  }

 private:
  Rectangle get_icon_frame(int index, Camera2D const& camera) const {
    Vector2 frame_pos = GetScreenToWorld2D(
        Vector2(COMMAND_ICON_PADDING, COMMAND_ICON_PADDING + (COMMAND_ICON_PADDING + COMMAND_ICON_SIZE) * index),
        camera);
    return Rectangle(frame_pos.x, frame_pos.y, COMMAND_ICON_SIZE, COMMAND_ICON_SIZE);
  }
};

struct Countdown {
  Countdown(float duration_seconds) : duration_seconds(duration_seconds) {
  }

  void reset() {
    counter = 0.0f;
  }

  void reset(float new_duration_seconds) {
    duration_seconds = new_duration_seconds;
    reset();
  }

  void update() {
    just_finished = false;

    if (counter < duration_seconds) {
      counter += GetFrameTime();

      if (counter > duration_seconds) {
        just_finished = true;
      }
    }
  }

  bool is_just_finished() const {
    return just_finished;
  }

  bool is_finished() const {
    return counter >= duration_seconds;
  }

 private:
  float duration_seconds;
  float counter{0.0f};
  bool just_finished{false};
};
