#pragma once

struct Config {
  int monitor_fps = 60;
  float get_frame_time() const;
  void set_time_multiplier(float v);
  float get_time_multiplier() const;

 private:
  float time_multiplier{1.f};
};

inline Config config{};
