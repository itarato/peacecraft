#pragma once

#include <ctype.h>

#include <cmath>
#include <cstdio>
#include <optional>
#include <set>
#include <source_location>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

#include "raylib.h"

constexpr int MOVE_GRID_SIZE = 35;

constexpr int COMMAND_ICON_SIZE = 60;
constexpr int COMMAND_ICON_PADDING = 10;

constexpr int RESOURCE_COUNT = 2;
const char* RESOURCE_NAMES[RESOURCE_COUNT] = {"Mineral", "Wood"};
constexpr int RESOURCE_MINERAL = 0;
constexpr int RESOURCE_WOOD = 1;

template <typename... Args>
[[noreturn]] inline void bail(const char* fmt, Args&&... args,
                              const std::source_location& loc = std::source_location::current()) {
  printf("\x1b[93m%16s\x1b[39m:\x1b[96m%-4d\x1b[0m \x1b[94m", loc.file_name(), loc.line());
  if constexpr (sizeof...(Args) > 0) {
    printf(fmt, std::forward<Args>(args)...);
  } else {
    fputs(fmt, stdout);
  }
  printf("\x1b[0m\n");
  std::exit(EXIT_FAILURE);
}

struct Vector2Int {
  int x{};
  int y{};

  Vector2Int() = default;

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

template <typename T>
void cleanup_removables(std::vector<T>& list) {
  list.erase(std::remove_if(list.begin(), list.end(), [](const auto& elem) { return elem.is_removable(); }),
             list.end());
}

template <typename T>
void cleanup_removables_sptr(std::vector<std::shared_ptr<T>>& list) {
  list.erase(std::remove_if(list.begin(), list.end(), [](const auto& elem) { return elem->is_removable(); }),
             list.end());
}

template <typename K, typename V>
void cleanup_removables_uomap(std::unordered_map<K, V>& list) {
  std::erase_if(list, [](const auto& kv) { return kv.second.is_removable(); });
}
