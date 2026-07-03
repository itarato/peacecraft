#pragma once

#include <algorithm>
#include <deque>
#include <memory>
#include <numeric>
#include <unordered_set>

#include "area_selector.h"
#include "buildings.h"
#include "characters.h"
#include "commands.h"
#include "config.h"
#include "grid_explorer.h"
#include "raylib.h"
#include "resource.h"
#include "universal_entity.h"
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

    buildings.emplace_back(Vector2{500.f, 500.f}, 1.0f);

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
  std::vector<std::shared_ptr<UniversalEntity>> universal_entities{};
  std::vector<Resource> resources{};
  AreaSelector selector{};
  Camera2D camera{};
  std::deque<CommandVariant> command_queue{};
  int resource_amounts[RESOURCE_COUNT] = {};

  void update() {
    update_command_selection();

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
    for (auto& c : characters) c.update(camera, characters, buildings);
    for (auto& b : buildings) b.update(camera);

    for (auto& u : universal_entities) {
      auto commands = u->update(camera);
      for (const auto& command : commands) execute_command(command);
    }

    cleanup_removables(characters);

    while (!command_queue.empty()) {
      execute_command(command_queue.front());
      command_queue.pop_front();
    }

    cleanup_removables_sptr(universal_entities);
  }

  void draw() const {
    ClearBackground(RAYWHITE);

    // Unit drawings.
    for (const auto& b : buildings) b.draw();
    for (const auto& c : characters) c.draw();
    for (const auto& u : universal_entities) u->draw(camera);

    selector.draw();

    draw_commands();
    draw_resources();

    Vector2 fps_pos = GetScreenToWorld2D(Vector2(10, GetScreenHeight() - 20), camera);
    DrawFPS(fps_pos.x, fps_pos.y);
  }

  void draw_commands() const {
    for (auto const& building : buildings) {
      if (building.is_selected()) {
        building.commands().draw(camera);
        return;
      }
    }

    int selected_character_count = std::accumulate(characters.begin(), characters.end(), 0, [](int acc, const auto& c) {
      return acc + (c.is_selected() ? 1 : 0);
    });

    if (selected_character_count == 1) {
      for (auto const& character : characters) {
        if (character.is_selected()) {
          character.commands().draw(camera);
          return;
        }
      }
    }
  }

  void draw_resources() const {
    int offset = 10;

    for (int i = 0; i < RESOURCE_COUNT; i++) {
      const char* label = TextFormat("%s: %d | ", RESOURCE_NAMES[i], resource_amounts[i]);
      int width = MeasureText(label, 20);
      Vector2 pos = GetScreenToWorld2D(Vector2(offset, 10), camera);
      DrawText(label, pos.x, pos.y, 20, BLACK);

      offset += width;
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

  void update_command_selection() {
    for (const auto& building : buildings) {
      if (building.is_selected()) {
        auto maybe_command = building.commands().just_selected_command(camera);
        if (maybe_command.has_value()) command_queue.push_back(std::move(maybe_command.value()));
        break;
      }
    }

    for (const auto& character : characters) {
      if (character.is_selected()) {
        auto maybe_command = character.commands().just_selected_command(camera);
        if (maybe_command.has_value()) command_queue.push_back(std::move(maybe_command.value()));
        break;
      }
    }
  }

  void execute_command(const CommandVariant cv) {
    if (std::holds_alternative<CharacterCreationCommand>(cv)) {
      CharacterCreationCommand command = std::get<CharacterCreationCommand>(cv);
      Vector2Int base_grid_pos = vector2_to_grid_pos(command.base_pos);
      GridPosExplorer gpe = GridPosExplorer(base_grid_pos, base_grid_pos, get_occupied_grid());
      Vector2Int available_grid_pos = gpe.next_available();
      Vector2 available_pos = grid_pos_to_vector2(available_grid_pos);
      characters.emplace_back(available_pos, PLAYER_CHARACTER_GROUP);

    } else if (std::holds_alternative<BuildingCreationRequestCommand>(cv)) {
      BuildingCreationRequestCommand command = std::get<BuildingCreationRequestCommand>(cv);
      universal_entities.push_back(
          std::make_shared<BuildingMarkerUEntity>(BuildingMarkerUEntity(command.character_id)));

    } else if (std::holds_alternative<BuildingCreationCommand>(cv)) {
      BuildingCreationCommand command = std::get<BuildingCreationCommand>(cv);
      buildings.emplace_back(command.pos);

    } else if (std::holds_alternative<CharacterMoveCommand>(cv)) {
      CharacterMoveCommand command = std::get<CharacterMoveCommand>(cv);

      for (auto& c : characters) {
        if (c.id == command.character_id) {
          c.set_move_target(command.target);
          break;
        }
      }
    } else {
      UNEXPECTED;
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
