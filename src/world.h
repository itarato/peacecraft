#pragma once

#include <unordered_map>
#include <vector>

#include "characters.h"
#include "group.h"
#include "resource.h"

struct World {
  virtual ~World() = default;

  virtual std::unordered_map<unsigned int, Character>& get_characters() = 0;
  virtual std::unordered_map<unsigned int, Resource>& get_resources() = 0;
  virtual std::vector<Group>& get_groups() = 0;
  virtual std::unordered_set<Vector2Int> get_occupied_grid() const = 0;
};
