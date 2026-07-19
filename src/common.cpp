#include "common.h"

#include <ctype.h>

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

bool Vector2Int::operator==(const Vector2Int& other) const {
  return x == other.x && y == other.y;
}

float vec2int_distance(Vector2Int a, Vector2Int b) {
  float sqx = std::powf((float)(a.x - b.x), 2.f);
  float sqy = std::powf((float)(a.y - b.y), 2.f);
  return std::sqrtf(sqx + sqy);
}

Vector2Int vector2_to_grid_pos(Vector2 v) {
  return Vector2Int((v.x + (MOVE_GRID_SIZE >> 1)) / MOVE_GRID_SIZE, (v.y + (MOVE_GRID_SIZE >> 1)) / MOVE_GRID_SIZE);
}

Vector2 grid_pos_to_vector2(Vector2Int v) {
  return Vector2(v.x * MOVE_GRID_SIZE, v.y * MOVE_GRID_SIZE);
}

void Countdown::reset() {
  counter = 0.0f;
}

void Countdown::reset(float new_duration_seconds) {
  duration_seconds = new_duration_seconds;
  reset();
}

void Countdown::update() {
  just_finished = false;

  if (counter < duration_seconds) {
    counter += GetFrameTime();

    if (counter > duration_seconds) {
      just_finished = true;
    }
  }
}

bool Countdown::is_just_finished() const {
  return just_finished;
}

bool Countdown::is_finished() const {
  return counter >= duration_seconds;
}

void Countdown::finish() {
  counter = duration_seconds;
}

float calculate_need_linear(float min, float max, float current) {
  if (current <= min) return 1.f;
  if (current >= max) return 0.f;
  return (current - min) / (max - min);
}
