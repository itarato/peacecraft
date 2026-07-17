#pragma once

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
#include "world.h"

enum class ResourceAutomationState {
  ReadyToStart,
  MoveToResource,
  Harvest,
  MoveToBase,
  Dump,
};

struct Automation {
  virtual ~Automation() = default;
  virtual void update(World* world) = 0;
  virtual const unsigned int get_character_id() const = 0;
  virtual bool is_removable() const = 0;

  std::shared_ptr<Automation> from_command(CommandVariant command) const;
};

struct CharacterCreationAutomation : Automation {
  CharacterCreationAutomation(Vector2 base_pos, int group_id) : base_pos(base_pos), group_id(group_id) {
  }

  void update(World* world) override {
    Vector2Int base_grid_pos = vector2_to_grid_pos(base_pos);
    GridPosExplorer gpe = GridPosExplorer(base_grid_pos, base_grid_pos, world->get_occupied_grid());
    Vector2Int available_grid_pos = gpe.next_available();
    Vector2 available_pos = grid_pos_to_vector2(available_grid_pos);
    Character new_character{available_pos, group_id};
    world->get_characters().emplace(new_character.id, std::move(new_character));

    completed = true;
  }

  const unsigned int get_character_id() const override {
    return INVALID_CHARACTER_ID;
  }

  bool is_removable() const {
    return completed;
  }

 private:
  Vector2 base_pos;
  int group_id;
  bool completed{false};
};

struct ResourceAutomation : Automation {
  unsigned int character_id;

  ResourceAutomation(const unsigned int character_id, const unsigned int resource_id, const Vector2 base_pos,
                     int group_id)
      : character_id(character_id), resource_id(resource_id), base_pos(base_pos) {
  }

  const unsigned int get_character_id() const override {
    return character_id;
  }

  void update(World* world) override {
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

  [[nodiscard]] bool is_removable() const override {
    return removable;
  }

 private:
  unsigned int resource_id;
  Vector2 base_pos;
  int group_id;
  ResourceAutomationState state{ResourceAutomationState::ReadyToStart};
  bool removable{false};
};

struct MoveAutomation : Automation {
  MoveAutomation(unsigned int character_id, Vector2 target) : character_id(character_id), target(target) {
  }

  void update(World* world) override {
    if (!world->get_characters().contains(character_id)) removable = true;
    if (removable) return;

    if (wait_phase) {
      if (world->get_characters().at(character_id).pos == target) removable = true;
    } else {
      world->get_characters().at(character_id).set_move_target(target);
      wait_phase = true;
    }
  }

  const unsigned int get_character_id() const override {
    return character_id;
  }

  bool is_removable() const override {
    return removable;
  }

 private:
  unsigned int character_id;
  Vector2 target;
  bool removable{false};
  bool wait_phase{false};
};

struct AutomationSequence : Automation {
  std::vector<std::shared_ptr<Automation>> automations{};

  void update(World* world) override {
    if (automations.empty()) return;

    if (automations[0]->is_removable()) {
      automations.erase(automations.begin());
      return;
    }

    automations[0]->update(world);
  }

  const unsigned int get_character_id() const override {
    if (automations.empty()) bail("No more automations");

    return automations[0]->get_character_id();
  }

  bool is_removable() const override {
    return automations.empty();
  }
};

struct BuildingAutomation : Automation {
  BuildingAutomation(unsigned int character_id) : character_id(character_id) {
  }

  void update(World* world) override {
    bail("todo");
  }

  const unsigned int get_character_id() const override {
    return character_id;
  }

  bool is_removable() const override {
    return removable;
  }

 private:
  unsigned int character_id;
  bool removable{false};
};

struct BuildingRequestAutomation : Automation {
  BuildingRequestAutomation(unsigned int character_id) : character_id(character_id) {
  }

  void update(World* world) override {
    world->get_universal_entities().push_back(
        std::make_shared<BuildingMarkerUEntity>(BuildingMarkerUEntity(character_id)));
    completed = true;
  }

  const unsigned int get_character_id() const override {
    return character_id;
  }

  bool is_removable() const override {
    return completed;
  }

 private:
  unsigned int character_id;
  bool completed{false};
};

std::shared_ptr<Automation> Automation::from_command(CommandVariant command) const {
  if (std::holds_alternative<CharacterCreationCommand>(command)) {
    CharacterCreationCommand command_instance = std::get<CharacterCreationCommand>(command);
    return std::make_shared<CharacterCreationAutomation>(command_instance.base_pos, PLAYER_CHARACTER_GROUP);
  }

  if (std::holds_alternative<CharacterMoveCommand>(command)) {
    CharacterMoveCommand command_instance = std::get<CharacterMoveCommand>(command);
    return std::make_shared<MoveAutomation>(command_instance.character_id, command_instance.target);
  }

  bail("Unhandled");
}

/**

- have a type (data: character + resource)
- update
  - tell character to go to position
  - make character harvest resource
    - detect when to finish: character has enough resource OR resource is exhausted
  - tell character to go to a base (find the closest)
  - [optional] wait
  - repeat

**/
