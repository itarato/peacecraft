#include "automations.h"

#include <algorithm>
#include <memory>
#include <numeric>
#include <unordered_map>
#include <variant>
#include <vector>

#include "characters.h"
#include "commands.h"
#include "grid_explorer.h"
#include "group.h"
#include "raylib.h"
#include "resource.h"
#include "universal_entity.h"
#include "world.h"

std::shared_ptr<Automation> Automation::from_command(CommandVariant command) {
  if (std::holds_alternative<CharacterCreationCommand>(command)) {
    CharacterCreationCommand command_instance = std::get<CharacterCreationCommand>(command);
    return std::make_shared<CharacterCreationAutomation>(command_instance.building_id);
  }

  if (std::holds_alternative<BuildingCreationRequestCommand>(command)) {
    BuildingCreationRequestCommand command_instance = std::get<BuildingCreationRequestCommand>(command);
    return std::make_shared<BuildingRequestAutomation>(command_instance.character_id);
  }

  bail("Unhandled command to automation conversion");
}

void CharacterCreationAutomation::update(World* world) {
  if (!world->get_buildings().contains(building_id)) {
    INFO("Building not found");
    completed = true;
    return;
  }
  Building& building = world->get_buildings().at(building_id);

  if (!world->get_groups()[building.group].can_pay_for(Payable::Character)) {
    completed = true;
    return;
  }

  if (!world->get_buildings().contains(building_id)) {
    completed = true;
    return;
  }

  Vector2Int base_grid_pos = vector2_to_grid_pos(building.pos);
  GridPosExplorer gpe = GridPosExplorer(base_grid_pos, base_grid_pos, world->get_chracter_occupied_grid());
  Vector2Int available_grid_pos = gpe.next_available();
  Vector2 available_pos = grid_pos_to_vector2(available_grid_pos);
  Character new_character{available_pos, building.group};
  world->get_characters().emplace(new_character.id, std::move(new_character));

  world->get_groups()[building.group].pay_for(Payable::Character);
  completed = true;
}

const unsigned int CharacterCreationAutomation::get_character_id() const {
  return INVALID_ID;
}

bool CharacterCreationAutomation::is_removable() const {
  return completed;
}

float CharacterCreationAutomation::priority() const {
  return 1.0f;
}

const unsigned int ResourceAutomation::get_character_id() const {
  return character_id;
}

void ResourceAutomation::update(World* world) {
  if (completed) return;

  if (!world->get_characters().contains(character_id)) {
    completed = true;
    return;
  }
  if (!world->get_resources().contains(resource_id)) {
    completed = true;
    return;
  }
  if (!world->get_buildings().contains(building_id)) {
    completed = true;
    return;
  }

  Character& owner = world->get_characters().at(character_id);
  Resource& resource = world->get_resources().at(resource_id);
  Building& building = world->get_buildings().at(building_id);

  switch (state) {
    case ResourceAutomationState::ReadyToStart:
      if (iterations_left <= 0) {
        completed = true;
        break;
      }

      owner.set_move_target(resource.pos);
      state = ResourceAutomationState::MoveToResource;
      break;
    case ResourceAutomationState::MoveToResource:
      if (owner.pos == resource.pos) state = ResourceAutomationState::Harvest;
      break;
    case ResourceAutomationState::Harvest: {
      const int amount = resource.harvest(10);
      owner.receive_resource(amount, resource.kind);
      owner.set_move_target(building.pos);
      state = ResourceAutomationState::MoveToBase;
      break;
    }
    case ResourceAutomationState::MoveToBase:
      if (owner.pos == building.pos) state = ResourceAutomationState::Dump;
      break;
    case ResourceAutomationState::Dump:
      for (int i = 0; i < RESOURCE_COUNT; i++) {
        world->get_groups()[group].resource_amounts[i] += owner.resource_amount(i);
      }
      owner.empty_resources();

      iterations_left--;

      state = ResourceAutomationState::ReadyToStart;
      break;
    default:
      bail("Unexpected");
  }
}

[[nodiscard]] bool ResourceAutomation::is_removable() const {
  return completed;
}

float ResourceAutomation::priority() const {
  return 1.0f;
}

void MoveAutomation::update(World* world) {
  if (!world->get_characters().contains(character_id)) completed = true;
  if (completed) return;

  if (wait_phase) {
    if (world->get_characters().at(character_id).pos == target) completed = true;
  } else {
    world->get_characters().at(character_id).set_move_target(target);
    wait_phase = true;
  }
}

const unsigned int MoveAutomation::get_character_id() const {
  return character_id;
}

bool MoveAutomation::is_removable() const {
  return completed;
}

float MoveAutomation::priority() const {
  return 0.5f;
}

void AutomationSequence::update(World* world) {
  if (automations.empty()) return;

  if (automations[0]->is_removable()) {
    automations.erase(automations.begin());
    return;
  }

  automations[0]->update(world);
}

const unsigned int AutomationSequence::get_character_id() const {
  if (character_id != INVALID_ID) return character_id;
  if (automations.empty()) bail("No more automations");

  return automations[0]->get_character_id();
}

bool AutomationSequence::is_removable() const {
  return automations.empty();
}

float AutomationSequence::priority() const {
  return std::accumulate(automations.begin(), automations.end(), 0.f,
                         [](float acc, auto const& a) { return std::max(acc, a->priority()); });
}

void BuildingAutomation::update(World* world) {
  if (!world->get_groups()[group].can_pay_for(Payable::Building)) {
    TraceLog(LOG_INFO, "Not enough resource for group %d to build", group);
    completed = true;
    return;
  }

  Building building{group, pos};
  world->get_buildings().emplace(building.id, std::move(building));

  world->get_groups()[group].pay_for(Payable::Building);

  completed = true;
}

const unsigned int BuildingAutomation::get_character_id() const {
  return INVALID_ID;
}

bool BuildingAutomation::is_removable() const {
  return completed;
}

float BuildingAutomation::priority() const {
  return 1.0f;
}

void BuildingRequestAutomation::update(World* world) {
  world->get_universal_entities().push_back(
      std::make_shared<BuildingMarkerUEntity>(BuildingMarkerUEntity(character_id)));
  completed = true;
}

const unsigned int BuildingRequestAutomation::get_character_id() const {
  return character_id;
}

bool BuildingRequestAutomation::is_removable() const {
  return completed;
}

float BuildingRequestAutomation::priority() const {
  return 1.0f;
}

[[nodiscard]] const unsigned int WaitForBuildingToBeReadyAutomation::get_character_id() const {
  return INVALID_ID;
}

bool WaitForBuildingToBeReadyAutomation::is_removable() const {
  return completed;
}

void WaitForBuildingToBeReadyAutomation::update(World* world) {
  if (building_id < 0) {
    building_id = last_building_id(world);
  }
  if (building_id < 0) {
    completed = true;
    return;
  }

  if (!world->get_buildings().contains(building_id)) completed = true;
  if (completed) return;

  if (world->get_buildings().at(building_id).is_complete()) {
    completed = true;
  }
}

int WaitForBuildingToBeReadyAutomation::last_building_id(World* world) const {
  int last_id{-1};
  for (auto const& [_id, b] : world->get_buildings()) {
    if (b.group != group) continue;

    if (static_cast<int>(b.id) > last_id) last_id = b.id;
  }

  return last_id;
}

float WaitForBuildingToBeReadyAutomation::priority() const {
  return 1.0f;
}

[[nodiscard]] const unsigned int ChaseAutomation::get_character_id() const {
  return character_id;
}

[[nodiscard]] bool ChaseAutomation::is_removable() const {
  return completed;
}

void ChaseAutomation::update(World* world) {
  if (!world->get_characters().contains(character_id)) {
    completed = true;
    return;
  }

  if (!world->get_characters().contains(target)) {
    completed = true;
    return;
  }

  world->get_characters().at(character_id).set_move_target(world->get_characters().at(target).pos);
}

float ChaseAutomation::priority() const {
  return 2.0f;
}
