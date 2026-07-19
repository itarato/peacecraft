#include "orchestrator.h"

#include <algorithm>
#include <unordered_map>
#include <utility>
#include <vector>

#include "automations.h"
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

constexpr float BUILDING_NEED_PRIORITY{1.3f};
constexpr float RESOURCE_WOOD_NEED_PRIORITY{1.5f};
constexpr float RESOURCE_MINERAL_NEED_PRIORITY{1.5f};
constexpr float CHARACTER_NEED_PRIORITY{1.6f};
constexpr float DEFENSE_NEED_PRIORITY{2.f};

void Orchestrator::update(World* world) {
  world_evaluation_countdown.update();

  if (world_evaluation_countdown.is_just_finished()) {
    float building_need = calculate_building_need(world->get_buildings(), world->get_groups()[group]);
    float wood_resource_needs = calculate_resource_gather_need(world->get_groups()[group], RESOURCE_WOOD);
    float mineral_resource_needs = calculate_resource_gather_need(world->get_groups()[group], RESOURCE_MINERAL);
    float character_need = calculate_character_need();
    auto defense_need = calculate_defense_need(world);

    TraceLog(LOG_INFO, "Building=%.2f Wood=%.2f Mineral=%.2f Character=%.2f Defense=%.2f", building_need,
             wood_resource_needs, mineral_resource_needs, character_need, defense_need.first);

    std::vector<std::pair<int, float>> needs{
        {static_cast<int>(NeedTag::BUILDING), building_need * BUILDING_NEED_PRIORITY},
        {static_cast<int>(NeedTag::RESOURCE_WOOD), wood_resource_needs * RESOURCE_WOOD_NEED_PRIORITY},
        {static_cast<int>(NeedTag::RESOURCE_MINERAL), mineral_resource_needs * RESOURCE_MINERAL_NEED_PRIORITY},
        {static_cast<int>(NeedTag::CHARACTER), character_need * CHARACTER_NEED_PRIORITY},
        {static_cast<int>(NeedTag::DEFENSE), defense_need.first * DEFENSE_NEED_PRIORITY},
    };
    std::sort(needs.begin(), needs.end(), [](auto const& lhs, auto const& rhs) { return lhs.second > rhs.second; });

    std::vector<unsigned int> available_characters{};
    for (auto const& [_id, c] : world->get_characters()) {
      if (c.group == group) available_characters.push_back(c.id);
    }

    for (auto const& [tag, score] : needs) {
      switch (tag) {
        case static_cast<int>(NeedTag::BUILDING):
          if (!available_characters.empty()) {
            // TODO: Smarter pick: someone not occuppied.
            auto id = available_characters.back();
            available_characters.pop_back();

            // TODO: Check if we have enough resources.
            auto pos = world->get_characters().at(id).pos;
            world->push_automation(std::make_shared<BuildingAutomation>(pos, group));
          }

          break;
        case static_cast<int>(NeedTag::RESOURCE_WOOD):
          break;
        case static_cast<int>(NeedTag::RESOURCE_MINERAL):
          break;
        case static_cast<int>(NeedTag::CHARACTER):
          break;
        case static_cast<int>(NeedTag::DEFENSE):
          break;
        default:
          break;
      }
    }

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

  return {calculate_inverse_need_linear(0.5f, 2.f, static_cast<float>(under_attack_character_ids.size())),
          under_attack_character_ids};
}
