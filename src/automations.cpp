#include "automations.h"

#include <memory>
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

void CharacterCreationAutomation::update(World* world) {
  Vector2Int base_grid_pos = vector2_to_grid_pos(base_pos);
  GridPosExplorer gpe = GridPosExplorer(base_grid_pos, base_grid_pos, world->get_occupied_grid());
  Vector2Int available_grid_pos = gpe.next_available();
  Vector2 available_pos = grid_pos_to_vector2(available_grid_pos);
  Character new_character{available_pos, group_id};
  world->get_characters().emplace(new_character.id, std::move(new_character));

  completed = true;
}

const unsigned int CharacterCreationAutomation::get_character_id() const {
  return INVALID_CHARACTER_ID;
}

bool CharacterCreationAutomation::is_removable() const {
  return completed;
}

const unsigned int ResourceAutomation::get_character_id() const {
  return character_id;
}

void ResourceAutomation::update(World* world) {
  if (!world->get_characters().contains(character_id)) removable = true;
  if (!world->get_resources().contains(resource_id)) removable = true;
  if (removable) return;

  Character& owner = world->get_characters().at(character_id);
  Resource& resource = world->get_resources().at(resource_id);

  switch (state) {
    case ResourceAutomationState::ReadyToStart:
      owner.set_move_target(resource.pos);
      state = ResourceAutomationState::MoveToResource;
      break;
    case ResourceAutomationState::MoveToResource:
      if (owner.pos == resource.pos) state = ResourceAutomationState::Harvest;
      break;
    case ResourceAutomationState::Harvest: {
      const int amount = resource.harvest(10);
      owner.receive_resource(amount, resource.kind);
      owner.set_move_target(base_pos);
      state = ResourceAutomationState::MoveToBase;
      break;
    }
    case ResourceAutomationState::MoveToBase:
      if (owner.pos == base_pos) state = ResourceAutomationState::Dump;
      break;
    case ResourceAutomationState::Dump:
      for (int i = 0; i < RESOURCE_COUNT; i++)
        world->get_groups()[group_id].resource_amounts[i] += owner.resource_amount(i);
      owner.empty_resources();
      state = ResourceAutomationState::ReadyToStart;
      break;
    default:
      bail("Unexpected");
  }
}

[[nodiscard]] bool ResourceAutomation::is_removable() const {
  return removable;
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

void AutomationSequence::update(World* world) {
  if (automations.empty()) return;

  if (automations[0]->is_removable()) {
    automations.erase(automations.begin());
    return;
  }

  automations[0]->update(world);
}

const unsigned int AutomationSequence::get_character_id() const {
  if (automations.empty()) bail("No more automations");

  return automations[0]->get_character_id();
}

bool AutomationSequence::is_removable() const {
  return automations.empty();
}

void BuildingAutomation::update(World* world) {
  world->get_buildings().emplace_back(PLAYER_CHARACTER_GROUP, pos);
  world->get_groups()[group].resource_amounts[RESOURCE_MINERAL] -= 100;
  world->get_groups()[group].resource_amounts[RESOURCE_WOOD] -= 50;

  completed = true;
}

const unsigned int BuildingAutomation::get_character_id() const {
  return INVALID_CHARACTER_ID;
}

bool BuildingAutomation::is_removable() const {
  return completed;
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

std::shared_ptr<Automation> Automation::from_command(CommandVariant command) {
  if (std::holds_alternative<CharacterCreationCommand>(command)) {
    CharacterCreationCommand command_instance = std::get<CharacterCreationCommand>(command);
    return std::make_shared<CharacterCreationAutomation>(command_instance.base_pos, PLAYER_CHARACTER_GROUP);
  }

  if (std::holds_alternative<BuildingCreationRequestCommand>(command)) {
    BuildingCreationRequestCommand command_instance = std::get<BuildingCreationRequestCommand>(command);
    return std::make_shared<BuildingRequestAutomation>(command_instance.character_id);
  }

  bail("Unhandled command to automation conversion");
}
