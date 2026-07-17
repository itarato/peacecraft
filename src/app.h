#pragma once

#include <algorithm>
#include <deque>
#include <memory>
#include <numeric>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include "area_selector.h"
#include "automations.h"
#include "buildings.h"
#include "characters.h"
#include "commands.h"
#include "config.h"
#include "grid_explorer.h"
#include "group.h"
#include "raylib.h"
#include "resource.h"
#include "universal_entity.h"
#include "world.h"

struct App : World {
  App() = default;
  ~App() = default;

  void init() {
    SetTraceLogLevel(LOG_DEBUG);

    InitWindow(1600, 1200, "PeaceCraft");
    config.monitor_fps = GetMonitorRefreshRate(0);
    SetTargetFPS(config.monitor_fps);

    Character character0(Vector2{200.f, 200.f}, PLAYER_CHARACTER_GROUP);
    characters.emplace(character0.id, std::move(character0));
    Character character1(Vector2{800.f, 600.f}, PLAYER_CHARACTER_GROUP);
    characters.emplace(character1.id, std::move(character1));
    Character character2(Vector2{1200.f, 300.f}, PLAYER_CHARACTER_GROUP);
    characters.emplace(character2.id, std::move(character2));
    Character character3(Vector2{600.f, 100.f}, ENEMY_CHARACTER_GROUP);
    characters.emplace(character3.id, std::move(character3));

    buildings.emplace_back(PLAYER_CHARACTER_GROUP, Vector2{500.f, 500.f}, 1.0f);

    for (int i = 100; i <= 300; i += 50) {
      for (int j = 300; j <= 500; j += 50) {
        Resource wood(RESOURCE_WOOD, Vector2(i, j));
        resources.emplace(wood.id, wood);

        Resource mineral(RESOURCE_MINERAL, Vector2(i + 800, j + 200));
        resources.emplace(mineral.id, mineral);
      }
    }

    groups.emplace_back(PLAYER_CHARACTER_GROUP);
    groups.emplace_back(ENEMY_CHARACTER_GROUP);

    auto seq = std::make_shared<AutomationSequence>();
    seq->automations.push_back(std::make_shared<MoveAutomation>(3, Vector2(800.f, 800.f)));
    seq->automations.push_back(std::make_shared<MoveAutomation>(3, Vector2(400.f, 1200.f)));

    automations.emplace_back(seq);

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

  std::unordered_map<unsigned int, Character>& get_characters() override {
    return characters;
  }

  std::unordered_map<unsigned int, Resource>& get_resources() override {
    return resources;
  }

  std::vector<Group>& get_groups() override {
    return groups;
  }

  std::vector<std::shared_ptr<UniversalEntity>>& get_universal_entities() override {
    return universal_entities;
  }

  std::vector<Building>& get_buildings() override {
    return buildings;
  }

 private:
  std::unordered_map<unsigned int, Character> characters{};
  std::vector<Building> buildings{};
  std::vector<std::shared_ptr<UniversalEntity>> universal_entities{};
  std::unordered_map<unsigned int, Resource> resources{};
  AreaSelector selector{};
  Camera2D camera{};
  std::deque<CommandVariant> command_queue{};
  std::vector<std::shared_ptr<Automation>> automations{};
  std::vector<Group> groups{};

  void update() {
    update_command_selection();

    // TODO: Only trigger deselection when the click is strictly on game-world - not widgets (eg
    //       map commands).
    if (IsMouseButtonPressed(0)) {
      for (auto& [_id, c] : characters) c.deselect();
      for (auto& b : buildings) b.deselect();
    }

    update_selector();
    update_target_movement();
    update_map_drag();
    update_character_resource_harvest_initialization();

    // Unit updates.
    for (auto& [_id, c] : characters) c.update(camera, characters, buildings);
    for (auto& b : buildings) b.update(camera);
    for (auto& a : automations) a->update(this);

    for (auto& u : universal_entities) {
      auto commands = u->update(camera);
      for (const auto& command : commands) execute_command(command);
    }

    cleanup_removables_uomap(characters);
    cleanup_removables_uomap(resources);
    cleanup_removables_sptr(automations);

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
    for (const auto& [_id, c] : characters) c.draw();
    for (const auto& u : universal_entities) u->draw(camera);
    for (const auto& [_id, r] : resources) r.draw();

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

    int selected_character_count =
        std::accumulate(characters.begin(), characters.end(), 0,
                        [](int acc, const auto& kv) { return acc + (kv.second.is_selected() ? 1 : 0); });

    if (selected_character_count == 1) {
      for (auto const& [_id, character] : characters) {
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
      const char* label =
          TextFormat("%s: %d | ", RESOURCE_NAMES[i], groups[PLAYER_CHARACTER_GROUP].resource_amounts[i]);
      const int width = MeasureText(label, 20);
      auto [x, y] = GetScreenToWorld2D(Vector2(offset, 10), camera);
      DrawText(label, x, y, 20, BLACK);

      offset += width;
    }
  }

  void update_selector() {
    selector.update(camera);

    if (selector.just_selected()) {
      const Rectangle selection_frame = selector.selection_frame();
      for (auto& [_id, character] : characters) {
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

      for (auto& [_id, character] : characters) {
        if (character.is_selected()) {
          delete_automations_for_character(character.id);

          GridPosExplorer gpe = GridPosExplorer(character.grid_pos(), target_grid_pos, occupied_grid);
          Vector2Int available_grid_pos = gpe.next_available();
          Vector2 available_pos = grid_pos_to_vector2(available_grid_pos);
          occupied_grid.insert(available_grid_pos);
          character.set_move_target(available_pos);
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

    for (const auto& [_id, character] : characters) {
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
      Character new_character{available_pos, PLAYER_CHARACTER_GROUP};
      characters.emplace(new_character.id, std::move(new_character));

    } else if (std::holds_alternative<BuildingCreationRequestCommand>(cv)) {
      BuildingCreationRequestCommand command = std::get<BuildingCreationRequestCommand>(cv);
      universal_entities.push_back(
          std::make_shared<BuildingMarkerUEntity>(BuildingMarkerUEntity(command.character_id)));

    } else if (std::holds_alternative<BuildingCreationCommand>(cv)) {
      auto [pos, group] = std::get<BuildingCreationCommand>(cv);
      buildings.emplace_back(PLAYER_CHARACTER_GROUP, pos);
      groups[group].resource_amounts[RESOURCE_MINERAL] -= 100;
      groups[group].resource_amounts[RESOURCE_WOOD] -= 50;

    } else if (std::holds_alternative<CharacterMoveCommand>(cv)) {
      auto [target, character_id] = std::get<CharacterMoveCommand>(cv);

      for (auto& [_id, c] : characters) {
        if (c.id == character_id) {
          c.set_move_target(target);
          break;
        }
      }

    } else {
      bail("Unexpected");
    }
  }

  void update_map_drag() {
    if (IsMouseButtonDown(2)) {
      camera.offset = Vector2Add(camera.offset, GetMouseDelta());
    }
  }

  void update_character_resource_harvest_initialization() {
    if (!IsMouseButtonPressed(1)) return;
    const Resource* resource = resource_at_pos(GetScreenToWorld2D(GetMousePosition(), camera));
    if (resource == nullptr) return;
    if (buildings.empty()) return;

    for (const auto& [_id, c] : characters) {
      if (!c.is_selected()) continue;

      delete_automations_for_character(c.id);
      automations.emplace_back(std::make_shared<ResourceAutomation>(c.id, resource->id, buildings.at(0).pos, c.group));
    }
  }

  const Resource* resource_at_pos(Vector2 pos) const {
    for (const auto& [_id, r] : resources) {
      if (CheckCollisionPointRec(pos, r.frame())) {
        return &r;
      }
    }

    return nullptr;
  }

  void delete_automations_for_character(const unsigned int character_id) {
    std::erase_if(automations, [&](const auto& a) { return a->get_character_id() == character_id; });
  }

  std::unordered_set<Vector2Int> get_occupied_grid() const override {
    std::unordered_set<Vector2Int> occupied_grid{};

    for (const auto& [_id, unit] : characters) {
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
