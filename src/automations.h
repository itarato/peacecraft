#pragma once

#include <unordered_map>

#include "characters.h"
#include "raylib.h"
#include "resource.h"

enum class ResourceAutomationState {
  ReadyToStart,
  MoveToResource,
  Harvest,
  MoveToBase,
  Dump,
};

struct ResourceAutomation {
  unsigned int character_id;

  ResourceAutomation(const unsigned int character_id, const unsigned int resource_id, const Vector2 base_pos)
      : character_id(character_id), resource_id(resource_id), base_pos(base_pos) {
  }

  void update(std::unordered_map<unsigned int, Character>& characters,
              std::unordered_map<unsigned int, Resource>& resources, int* game_resource_amounts) {
    if (!characters.contains(character_id)) removable = true;
    if (!resources.contains(resource_id)) removable = true;
    if (removable) return;

    Character& owner = characters.at(character_id);
    Resource& resource = resources.at(resource_id);

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
        for (int i = 0; i < RESOURCE_COUNT; i++) game_resource_amounts[i] += owner.resource_amount(i);
        owner.empty_resources();
        state = ResourceAutomationState::ReadyToStart;
        break;
      default:
        bail("Unexpected");
    }
  }

  [[nodiscard]] bool is_removable() const {
    return removable;
  }

 private:
  unsigned int resource_id;
  Vector2 base_pos;
  ResourceAutomationState state{ResourceAutomationState::ReadyToStart};
  bool removable{false};
};

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
