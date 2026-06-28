#pragma once

#include <unordered_set>

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

    units.emplace_back(Vector2{200.f, 200.f});
    units.emplace_back(Vector2{800.f, 600.f});
    units.emplace_back(Vector2{1200.f, 300.f});
  }

  void run() {
    while (!WindowShouldClose()) {
      update();

      BeginDrawing();

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

    if (selector.just_selected()) {
      const Rectangle selection_frame = selector.selection_frame();
      for (auto& unit : units) {
        if (CheckCollisionRecs(selection_frame, unit.frame())) {
          unit.select();
        } else {
          unit.deselect();
        }
      }
    }

    if (IsMouseButtonPressed(1)) {
      std::unordered_set<Vector2Int> occuped_grid{};
      Vector2Int target_grid_pos = vector2_to_grid_pos(GetMousePosition());

      for (const auto& unit : units) {
        if (!unit.is_selected()) {
          occuped_grid.insert(unit.grid_pos());
        }
      }

      for (auto& unit : units) {
        if (unit.is_selected()) {
          GridPosExplorer gpe = GridPosExplorer(unit.grid_pos(), target_grid_pos, occuped_grid);
          Vector2Int available_grid_pos = gpe.next_available();
          Vector2 available_pos = grid_pos_to_vector2(available_grid_pos);
          occuped_grid.insert(available_grid_pos);
          unit.set_move_target(available_pos);
        }
      }
    }

    for (auto& unit : units) {
      unit.update();
    }
  }

  void draw() const {
    ClearBackground(RAYWHITE);

    for (const auto& unit : units) {
      unit.draw();
    }

    selector.draw();

    DrawFPS(10, GetScreenHeight() - 20);
  }
};
