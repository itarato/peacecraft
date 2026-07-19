#include "orchestrator.h"

#include <unordered_map>
#include <vector>

#include "buildings.h"
#include "characters.h"
#include "group.h"

enum class NeedTag {
  BUILDING,
  RESOURCE_WOOD,
  RESOURCE_MINERAL,
  CHARACTER,
  DEFENSE,
};

void Orchestrator::update(World* world) {
  world_evaluation_countdown.update();

  if (world_evaluation_countdown.is_just_finished()) {
    float building_need = calculate_building_need(world->get_buildings(), world->get_groups()[group]);
    float wood_resource_needs = calculate_resource_gather_need(world->get_groups()[group], RESOURCE_WOOD);
    float mineral_resource_needs = calculate_resource_gather_need(world->get_groups()[group], RESOURCE_MINERAL);
    float character_need = calculate_character_need();
    auto defense_need = calculate_defense_need(world);

    std::unordered_map<int, float> needs{
        {static_cast<int>(NeedTag::BUILDING), building_need},
        {static_cast<int>(NeedTag::RESOURCE_WOOD), wood_resource_needs},
        {static_cast<int>(NeedTag::RESOURCE_MINERAL), mineral_resource_needs},
        {static_cast<int>(NeedTag::CHARACTER), character_need},
        {static_cast<int>(NeedTag::DEFENSE), defense_need.first},
    };

    world_evaluation_countdown.reset();
  }
}

float Orchestrator::calculate_building_need(std::unordered_map<unsigned int, Building>& buildings, Group& group) const {
  int count = 0;
  for (const auto& [_id, b] : buildings) {
    if (b.group == group.id) count++;
  }

  return calculate_need_linear(2.f, 20.f, (float)count);
}

float Orchestrator::calculate_resource_gather_need(Group& group, int resource) const {
  return calculate_need_linear(200.f, 10000.f, group.resource_amounts[resource]);
}

float Orchestrator::calculate_character_need() const {
  return 0.8;
}

std::pair<float, std::vector<unsigned int>> Orchestrator::calculate_defense_need(World* world) const {
  std::vector<unsigned int> under_attack_character_ids{};

  for (auto const& [_id, c] : world->get_characters()) {
    if (c.group != group) continue;

    if (c.is_under_attack()) {
      under_attack_character_ids.push_back(c.id);
    }
  }

  return {calculate_need_linear(1.f, 2.f, static_cast<float>(under_attack_character_ids.size())),
          under_attack_character_ids};
}
