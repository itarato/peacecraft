#include "orchestrator.h"

#include <unordered_map>
#include <vector>

#include "buildings.h"
#include "characters.h"
#include "group.h"

void Orchestrator::update(std::unordered_map<unsigned int, Character>& characters, std::vector<Building>& buildings,
                          Group& group) {
  // float building_need = calculate_building_need(buildings, group);
  // std::vector<float> resource_needs = calculate_resource_gather_need(group);
  // float character_need = calculate_character_need();
}

float Orchestrator::calculate_building_need(std::vector<Building>& buildings, Group& group) const {
  int count = 0;
  for (const auto& b : buildings) {
    if (b.group == group.id) count++;
  }

  return calculate_need_linear(2.f, 20.f, (float)count);
}

std::vector<float> Orchestrator::calculate_resource_gather_need(Group& group) const {
  std::vector<float> needs{};

  for (const auto& resource_amount : group.resource_amounts) {
    needs.emplace_back(calculate_need_linear(200.f, 10000.f, resource_amount));
  }

  return needs;
}

float Orchestrator::calculate_character_need() const {
  return 0.8;
}
