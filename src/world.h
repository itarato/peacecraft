#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "buildings.h"
#include "characters.h"
#include "group.h"
#include "resource.h"

struct UniversalEntity;
struct Automation;

struct World {
  virtual ~World() = default;

  virtual std::unordered_map<unsigned int, Character>& get_characters() = 0;
  virtual std::unordered_map<unsigned int, Resource>& get_resources() = 0;
  virtual std::vector<std::shared_ptr<UniversalEntity>>& get_universal_entities() = 0;
  virtual std::unordered_map<unsigned int, Building>& get_buildings() = 0;
  virtual std::vector<std::shared_ptr<Automation>>& get_automations() = 0;
  virtual std::vector<Group>& get_groups() = 0;
  virtual std::unordered_set<Vector2Int> get_chracter_occupied_grid() const = 0;
  virtual std::unordered_set<Vector2Int> get_building_occupied_grid() const = 0;
  virtual unsigned int closest_building(int group, Vector2 pos) const = 0;
  virtual unsigned int closest_resource(int resource, Vector2 pos) const = 0;
};
