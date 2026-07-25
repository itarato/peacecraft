#include "orchestrator.h"

#include <algorithm>
#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "automations.h"
#include "buildings.h"
#include "characters.h"
#include "common.h"
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
    float building_need = calculate_building_need(world);
    float wood_resource_needs = calculate_resource_gather_need(world->get_groups()[group], RESOURCE_WOOD);
    float mineral_resource_needs = calculate_resource_gather_need(world->get_groups()[group], RESOURCE_MINERAL);
    float character_need = calculate_character_need();
    auto defense_need = calculate_defense_need(world);

    // TraceLog(LOG_INFO, "Building=%.2f Wood=%.2f Mineral=%.2f Character=%.2f Defense=%.2f", building_need,
    //          wood_resource_needs, mineral_resource_needs, character_need, defense_need.first);

    std::vector<std::pair<int, float>> needs{
        {static_cast<int>(NeedTag::BUILDING), building_need * BUILDING_NEED_PRIORITY},
        {static_cast<int>(NeedTag::RESOURCE_WOOD), wood_resource_needs * RESOURCE_WOOD_NEED_PRIORITY},
        {static_cast<int>(NeedTag::RESOURCE_MINERAL), mineral_resource_needs * RESOURCE_MINERAL_NEED_PRIORITY},
        {static_cast<int>(NeedTag::CHARACTER), character_need * CHARACTER_NEED_PRIORITY},
        {static_cast<int>(NeedTag::DEFENSE), defense_need.first * DEFENSE_NEED_PRIORITY},
    };
    std::sort(needs.begin(), needs.end(), [](auto const& lhs, auto const& rhs) { return lhs.second > rhs.second; });

    std::unordered_set<unsigned int> busy_characters{};
    for (auto const& a : world->get_automations()) {
      busy_characters.insert(a->get_character_id());
    }

    std::deque<unsigned int> available_characters{};
    for (auto const& [_id, c] : world->get_characters()) {
      if (c.group != group) continue;
      // TODO: instead of ignoring busy characters let's add a priority score to the automation and order the
      //       available list ASC by that.
      if (busy_characters.contains(c.id)) continue;

      available_characters.push_back(c.id);
    }

    // TraceLog(LOG_INFO, "Busy: %d Available: %d", busy_characters.size(), available_characters.size());

    for (auto const& [tag, score] : needs) {
      switch (tag) {
        case static_cast<int>(NeedTag::BUILDING): {
          // Have no available worker.
          if (available_characters.empty()) break;
          // Does not have enough resources.
          if (!world->get_groups()[group].can_pay_for(Payable::Building)) break;

          // TODO: Smarter pick: someone not occuppied.
          auto id = available_characters.front();
          available_characters.pop_front();

          // TODO: Check if we have enough resources.
          auto char_pos = vector2_to_grid_pos(world->get_characters().at(id).pos);
          auto occupied_grid = world->get_building_occupied_grid();
          auto building_grid_pos = GridPosExplorer(char_pos, char_pos, occupied_grid).next_available();
          auto building_pos = grid_pos_to_vector2(building_grid_pos);

          std::shared_ptr<AutomationSequence> building_automation = std::make_shared<AutomationSequence>(id);
          building_automation->automations.push_back(std::make_shared<MoveAutomation>(id, building_pos));
          building_automation->automations.push_back(std::make_shared<BuildingAutomation>(building_pos, group));
          building_automation->automations.push_back(std::make_shared<WaitForBuildingToBeReadyAutomation>(group));

          world->get_automations().push_back(building_automation);

          break;
        }

        case static_cast<int>(NeedTag::RESOURCE_MINERAL):
        case static_cast<int>(NeedTag::RESOURCE_WOOD): {
          int resource;
          switch (tag) {
            case static_cast<int>(NeedTag::RESOURCE_MINERAL):
              resource = RESOURCE_MINERAL;
              break;
            case static_cast<int>(NeedTag::RESOURCE_WOOD):
              resource = RESOURCE_WOOD;
              break;
            default:
              bail("Unhandled resource");
          }

          if (available_characters.empty()) break;

          unsigned int worker_id = available_characters.front();
          available_characters.pop_front();
          const auto& worker = world->get_characters().at(worker_id);

          unsigned int closest_building_id = world->closest_building(group, worker.pos);
          if (closest_building_id == INVALID_ID) {
            available_characters.push_front(worker_id);
            break;
          }

          unsigned int closest_resource_id = world->closest_resource(resource, worker.pos);
          if (closest_resource_id == INVALID_ID) {
            available_characters.push_front(worker_id);
            break;
          }

          world->get_automations().push_back(
              std::make_shared<ResourceAutomation>(worker_id, closest_resource_id, closest_building_id, group, 5));

          break;
        }

        case static_cast<int>(NeedTag::CHARACTER): {
          unsigned int building_id{INVALID_ID};
          for (auto const& [id, b] : world->get_buildings()) {
            if (b.group != group) continue;
            if (!b.is_complete()) continue;
            building_id = id;
            break;
          }

          if (building_id == INVALID_ID) break;

          world->get_automations().push_back(std::make_shared<CharacterCreationAutomation>(building_id));

          break;
        }

        case static_cast<int>(NeedTag::DEFENSE): {
          while (!defense_need.second.empty()) {
            // TODO: attackers could be ordered by proximity.
            auto attacker_id = defense_need.second.extract(defense_need.second.begin());
          }

          break;
        }

        default:
          bail("Unrecognized tag");
          break;
      }
    }

    world_evaluation_countdown.reset();
  }
}

float Orchestrator::calculate_building_need(World* world) const {
  int count = 0;
  for (const auto& [_id, b] : world->get_buildings()) {
    if (b.group == world->get_groups()[group].id) count++;
  }

  int population{0};
  for (const auto& [_id, c] : world->get_characters()) {
    if (c.group == group) population++;
  }

  return calculate_need_linear(2.f, std::max(3.f, static_cast<float>(population / 2)), (float)count);
}

float Orchestrator::calculate_resource_gather_need(Group& group, int resource) const {
  return calculate_need_linear(200.f, 10000.f, group.resource_amounts[resource]);
}

float Orchestrator::calculate_character_need() const {
  return 0.8;
}

//        Severity           Attacker ID
std::pair<float, std::unordered_set<unsigned int>> Orchestrator::calculate_defense_need(World* world) const {
  std::vector<unsigned int> under_attack_character_ids{};

  for (auto const& [_id, c] : world->get_characters()) {
    if (c.group != group) continue;

    if (c.is_under_attack()) {
      under_attack_character_ids.push_back(c.last_attacker_id);
    }
  }

  return {calculate_inverse_need_linear(0.5f, 2.f, static_cast<float>(under_attack_character_ids.size())),
          under_attack_character_ids};
}
