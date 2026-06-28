#pragma once

#include <unordered_set>

#include "config.h"
#include "grid_explorer.h"
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

    characters.emplace_back(Vector2{200.f, 200.f});
    characters.emplace_back(Vector2{800.f, 600.f});
    characters.emplace_back(Vector2{1200.f, 300.f});
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
  std::vector<Character> characters{};
  std::vector<Building> buildings{};
  Selector selector{};

  void update() {
    update_selector();
    update_target_movement();

    // Unit updates.
    for (auto& c : characters) c.update();
    for (auto& b : buildings) b.update();
  }

  void draw() const {
    ClearBackground(RAYWHITE);

    // Unit drawings.
    for (const auto& c : characters) c.draw();
    for (const auto& b : buildings) b.draw();

    selector.draw();

    DrawFPS(10, GetScreenHeight() - 20);
  }

  void update_selector() {
    selector.update();

    if (selector.just_selected()) {
      const Rectangle selection_frame = selector.selection_frame();
      for (auto& unit : characters) {
        if (CheckCollisionRecs(selection_frame, unit.frame())) {
          unit.select();
        } else {
          unit.deselect();
        }
      }
    }
  }

  void update_target_movement() {
    if (IsMouseButtonPressed(1)) {
      std::unordered_set<Vector2Int> occuped_grid{};
      Vector2Int target_grid_pos = vector2_to_grid_pos(GetMousePosition());

      for (const auto& unit : characters) {
        if (!unit.is_selected()) {
          occuped_grid.insert(unit.grid_pos());
        }
      }

      for (auto& unit : characters) {
        if (unit.is_selected()) {
          GridPosExplorer gpe = GridPosExplorer(unit.grid_pos(), target_grid_pos, occuped_grid);
          Vector2Int available_grid_pos = gpe.next_available();
          Vector2 available_pos = grid_pos_to_vector2(available_grid_pos);
          occuped_grid.insert(available_grid_pos);
          unit.set_move_target(available_pos);
        }
      }
    }
  }
};
