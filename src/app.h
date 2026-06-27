#pragma once

#include "config.h"
#include "raylib.h"
#include "selection.h"
#include "units.h"
#include "vector"

struct App {
  App() {
  }

  void init() {
    SetTraceLogLevel(LOG_DEBUG);

    InitWindow(1600, 800, "PeaceCraft");
    config.monitor_fps = GetMonitorRefreshRate(0);
    SetTargetFPS(config.monitor_fps);

    units.emplace_back(Vector2{100.f, 200.f});
  }

  void run() {
    while (!WindowShouldClose()) {
      update();

      BeginDrawing();

      ClearBackground(RAYWHITE);
      draw();

      EndDrawing();
    }

    CloseWindow();
  }

 private:
  std::vector<Unit> units{};
  Selector selector{};

  void update() {
    selector.update();

    for (auto& unit : units) {
      unit.update();
    }
  }

  void draw() const {
    for (const auto& unit : units) {
      unit.draw();
    }

    selector.draw();

    DrawFPS(10, GetScreenHeight() - 20);
  }
};
