#pragma once

#include <ctype.h>

#include <algorithm>
#include <climits>
#include <cmath>
#include <cstdio>
#include <memory>
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

constexpr unsigned int INVALID_CHARACTER_ID = std::numeric_limits<unsigned int>::max();

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

  bool operator==(const Vector2Int& other) const;
};

float vec2int_distance(Vector2Int a, Vector2Int b);

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

int pos_to_grid(float v);
float grid_to_pos(int v);
Vector2Int vector2_to_grid_pos(Vector2 v);
Vector2 grid_pos_to_vector2(Vector2Int v);

struct Countdown {
  Countdown(float duration_seconds) : duration_seconds(duration_seconds) {
  }

  void reset();
  void reset(float new_duration_seconds);
  void update();
  bool is_just_finished() const;
  bool is_finished() const;
  void finish();

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

float calculate_need_linear(float min, float max, float current);
float calculate_inverse_need_linear(float min, float max, float current);
