#include "config.h"

#include "raylib.h"

float Config::get_frame_time() const {
  return GetFrameTime() * time_multiplier;
}

void Config::set_time_multiplier(float v) {
  time_multiplier = v;
}

float Config::get_time_multiplier() const {
  return time_multiplier;
}
