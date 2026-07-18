#pragma once

#include <unordered_map>
#include <vector>

#include "buildings.h"
#include "characters.h"
#include "group.h"

struct Orchestrator {
  void update(std::unordered_map<unsigned int, Character>& characters, std::vector<Building>& buildings, Group& group);

 private:
  float calculate_building_need(std::vector<Building>& buildings, Group& group) const;
  std::vector<float> calculate_resource_gather_need(Group& group) const;
  float calculate_character_need() const;
};
