#pragma once

#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "buildings.h"
#include "common.h"
#include "group.h"
#include "world.h"

struct Orchestrator {
  Orchestrator(int group) : group(group) {
  }

  void update(World* world);

 private:
  int group;
  Countdown world_evaluation_countdown{1.f};

  float calculate_building_need(World* world) const;
  float calculate_resource_gather_need(Group& group, int resource) const;
  float calculate_character_need() const;
  std::pair<float, std::unordered_set<unsigned int>> calculate_defense_need(World* world) const;
};

// struct AvailableCharacter {
//   unsigned int character_id;
//   // The higher the priority the more important activity it's doing. 0 means it's fully free.
//   float priority;
// };
