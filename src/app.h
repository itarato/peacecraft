#pragma once

#include <algorithm>
#include <unordered_set>

#include "area_selector.h"
#include "buildings.h"
#include "characters.h"
#include "config.h"
#include "grid_explorer.h"
#include "raylib.h"
#include "vector"

struct App {
  App() {
  }

  void init() {
    SetTraceLogLevel(LOG_DEBUG);

    InitWindow(1600, 800, "PeaceCraft");
    config.monitor_fps = GetMonitorRefreshRate(0);
    SetTargetFPS(config.monitor_fps);

    characters.emplace_back(Vector2{200.f, 200.f}, PLAYER_CHARACTER_GROUP);
    characters.emplace_back(Vector2{800.f, 600.f}, PLAYER_CHARACTER_GROUP);
    characters.emplace_back(Vector2{1200.f, 300.f}, PLAYER_CHARACTER_GROUP);

    characters.emplace_back(Vector2{600.f, 100.f}, ENEMY_CHARACTER_GROUP);

    buildings.emplace_back(Vector2{500.f, 500.f});

    camera.zoom = 1.f;
  }

  void run() {
    while (!WindowShouldClose()) {
      update();

      BeginDrawing();
      BeginMode2D(camera);

      draw();

      EndMode2D();
      EndDrawing();
    }

    CloseWindow();
  }

 private:
  std::vector<Character> characters{};
  std::vector<Building> buildings{};
  AreaSelector selector{};
  Camera2D camera{};

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
    update_map_drag();

    // Unit updates.
    for (auto& c : characters) c.update(camera, characters);
    for (auto& b : buildings) b.update(camera);

    characters.erase(std::remove_if(characters.begin(), characters.end(),
                                    [](const auto& character) { return character.is_removable(); }),
                     characters.end());
  }

  void draw() const {
    ClearBackground(RAYWHITE);

    // Unit drawings.
    for (const auto& b : buildings) b.draw();
    for (const auto& c : characters) c.draw();

    selector.draw();

    draw_commands();

    Vector2 fps_pos = GetScreenToWorld2D(Vector2(10, GetScreenHeight() - 20), camera);
    DrawFPS(fps_pos.x, fps_pos.y);
  }

  void draw_commands() const {
    for (auto const& building : buildings) {
      if (building.is_selected()) {
        building.commands().draw(camera);
        break;
      }
    }
  }

  void update_selector() {
    selector.update(camera);

    if (selector.just_selected()) {
      const Rectangle selection_frame = selector.selection_frame();
      for (auto& character : characters) {
        if (character.is_selectable() && CheckCollisionRecs(selection_frame, character.frame())) {
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
      Vector2Int target_grid_pos = vector2_to_grid_pos(GetScreenToWorld2D(GetMousePosition(), camera));

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
        auto maybe_command = building.commands().just_selected_command(camera);
        if (maybe_command.has_value()) {
          CommandVariant command_variant = maybe_command.value();

          if (std::holds_alternative<CharacterCreationCommand>(command_variant)) {
            CharacterCreationCommand command = std::get<CharacterCreationCommand>(command_variant);
            Vector2Int base_grid_pos = vector2_to_grid_pos(command.base_pos);
            GridPosExplorer gpe = GridPosExplorer(base_grid_pos, base_grid_pos, get_occupied_grid());
            Vector2Int available_grid_pos = gpe.next_available();
            Vector2 available_pos = grid_pos_to_vector2(available_grid_pos);
            characters.emplace_back(available_pos, PLAYER_CHARACTER_GROUP);
          } else {
            UNEXPECTED;
          }
        }
        break;
      }
    }
  }

  void update_map_drag() {
    if (IsMouseButtonDown(2)) {
      camera.offset = Vector2Add(camera.offset, GetMouseDelta());
    }
  }

  std::unordered_set<Vector2Int> get_occupied_grid() const {
    std::unordered_set<Vector2Int> occupied_grid{};

    for (const auto& unit : characters) {
      if (!unit.is_selected()) {
        occupied_grid.insert(unit.grid_pos());
      }
    }

    for (const auto& building : buildings) {
      occupied_grid.insert(building.grid_pos());
    }

    return occupied_grid;
  }
};
