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

  void init();
  void run();
  std::unordered_map<unsigned int, Character>& get_characters() override;
  std::unordered_map<unsigned int, Resource>& get_resources() override;
  std::vector<Group>& get_groups() override;
  std::vector<std::shared_ptr<UniversalEntity>>& get_universal_entities() override;
  std::vector<Building>& get_buildings() override;

 private:
  std::unordered_map<unsigned int, Character> characters{};
  std::vector<Building> buildings{};
  std::vector<std::shared_ptr<UniversalEntity>> universal_entities{};
  std::unordered_map<unsigned int, Resource> resources{};
  AreaSelector selector{};
  Camera2D camera{};
  std::vector<std::shared_ptr<Automation>> automations{};
  std::vector<Group> groups{};

  void update();
  void draw() const;
  void draw_commands() const;
  void draw_resources() const;
  void update_selector();
  void update_target_movement();
  void update_command_selection();
  void execute_command(const CommandVariant cv);
  void update_map_drag();
  void update_character_resource_harvest_initialization();
  const Resource* resource_at_pos(Vector2 pos) const;
  void delete_automations_for_character(const unsigned int character_id);
  std::unordered_set<Vector2Int> get_occupied_grid() const override;
};
