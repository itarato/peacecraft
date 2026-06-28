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

    buildings.emplace_back(Vector2{500.f, 500.f});
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
  AreaSelector selector{};

  void update() {
    update_building_commands();

    // TODO: Only trigger deselection when the click is strictly on game-world - not widgets (eg
    // map commands).
    if (IsMouseButtonPressed(0)) {
      for (auto& c : characters) c.deselect();
      for (auto& b : buildings) b.deselect();
    }

    update_selector();
    update_target_movement();

    // Unit updates.
    for (auto& c : characters) c.update();
    for (auto& b : buildings) b.update();
  }

  void draw() const {
    ClearBackground(RAYWHITE);

    // Unit drawings.
    for (const auto& b : buildings) b.draw();
    for (const auto& c : characters) c.draw();

    selector.draw();

    draw_building_commands();

    DrawFPS(10, GetScreenHeight() - 20);
  }

  void draw_building_commands() const {
    for (auto const& building : buildings) {
      if (building.is_selected()) {
        building.buildind_commands.draw();
        break;
      }
    }
  }

  void update_selector() {
    selector.update();

    if (selector.just_selected()) {
      const Rectangle selection_frame = selector.selection_frame();
      for (auto& character : characters) {
        if (CheckCollisionRecs(selection_frame, character.frame())) {
          character.select();
        } else {
          character.deselect();
        }
      }
    }
  }

  void update_target_movement() {
    if (IsMouseButtonPressed(1)) {
      std::unordered_set<Vector2Int> occupied_grid{get_occupied_grid()};
      Vector2Int target_grid_pos = vector2_to_grid_pos(GetMousePosition());

      for (auto& unit : characters) {
        if (unit.is_selected()) {
          GridPosExplorer gpe = GridPosExplorer(unit.grid_pos(), target_grid_pos, occupied_grid);
          Vector2Int available_grid_pos = gpe.next_available();
          Vector2 available_pos = grid_pos_to_vector2(available_grid_pos);
          occupied_grid.insert(available_grid_pos);
          unit.set_move_target(available_pos);
        }
      }
    }
  }

  void update_building_commands() {
    for (auto& building : buildings) {
      if (building.is_selected()) {
        auto maybe_command = building.buildind_commands.selected_command();
        if (maybe_command.has_value()) {
          GameCommand command = maybe_command.value();
          switch (command.type) {
            case GameCommandType::CharacterCreation: {
              TraceLog(LOG_INFO, "Create char");

              Vector2Int base_grid_pos = vector2_to_grid_pos(command.character_creation_command.base_pos);
              GridPosExplorer gpe = GridPosExplorer(base_grid_pos, base_grid_pos, get_occupied_grid());
              Vector2Int available_grid_pos = gpe.next_available();
              Vector2 available_pos = grid_pos_to_vector2(available_grid_pos);
              characters.emplace_back(available_pos);

              break;
            }
            default:
              TraceLog(LOG_ERROR, "Unhandled command case");
              exit(EXIT_FAILURE);
          }
        }
        break;
      }
    }
  }

  std::unordered_set<Vector2Int> get_occupied_grid() const {
    std::unordered_set<Vector2Int> occupied_grid{};

    for (const auto& unit : characters) {
      if (!unit.is_selected()) {
        occupied_grid.insert(unit.grid_pos());
      }
    }

    return occupied_grid;
  }
};
